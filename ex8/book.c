#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int request = 0;
pthread_mutex_t mutex;
pthread_cond_t cond;
pthread_mutex_t print_mutex;  // Separate mutex for printing
int *seats;   // shared memory pointer

void* user_thread(void* arg)
{
    while(1)
    {
        int tickets;
        pthread_mutex_lock(&mutex);
        if(*seats == 0)
        {
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex);

        pthread_mutex_lock(&print_mutex);
        printf(">> Enter tickets to book: ");
        fflush(stdout);
        pthread_mutex_unlock(&print_mutex);

        scanf("%d",&tickets);

        pthread_mutex_lock(&mutex);
        request = tickets;
        pthread_mutex_lock(&print_mutex);
        printf("[USER] Request: %d ticket(s)\n",tickets);
        fflush(stdout);
        pthread_mutex_unlock(&print_mutex);
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
    return NULL;
}

void* booking_thread(void* arg)
{
    while(1)
    {
        pthread_mutex_lock(&mutex);
        while(request == 0 && *seats > 0)
            pthread_cond_wait(&cond,&mutex);
        if(*seats == 0)
        {
            pthread_mutex_unlock(&mutex);
            break;
        }
        if(request <= *seats)
        {
            *seats -= request;
            pthread_mutex_lock(&print_mutex);
            printf("[BOOKING] Confirmed → %d ticket(s) booked\n",request);
            fflush(stdout);
            pthread_mutex_unlock(&print_mutex);
        }
        else
        {
            pthread_mutex_lock(&print_mutex);
            printf("[BOOKING] Rejected → Only %d seat(s) available\n",*seats);
            fflush(stdout);
            pthread_mutex_unlock(&print_mutex);
        }
        request = 0;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void* display_thread(void* arg)
{
    while(1)
    {
        pthread_mutex_lock(&mutex);
        pthread_mutex_lock(&print_mutex);
        printf("[DISPLAY] Available seats: %d\n",*seats);
        fflush(stdout);
        pthread_mutex_unlock(&print_mutex);
        if(*seats == 0)
        {
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex);
        sleep(2);
    }
    return NULL;
}

int main()
{
    pthread_t user, booking, display;
    pthread_mutex_init(&mutex,NULL);
    pthread_mutex_init(&print_mutex,NULL);
    pthread_cond_init(&cond,NULL);

    /* Shared Memory Creation */
    int shmid = shmget(IPC_PRIVATE, sizeof(int), 0666 | IPC_CREAT);
    seats = (int*) shmat(shmid, NULL, 0);
    *seats = 10;   // total seats

    pthread_create(&display,NULL,display_thread,NULL);
    pthread_create(&booking,NULL,booking_thread,NULL);
    pthread_create(&user,NULL,user_thread,NULL);

    pthread_join(user,NULL);
    pthread_join(booking,NULL);
    pthread_join(display,NULL);

    pthread_mutex_lock(&print_mutex);
    printf("\n=== All seats booked. System closing. ===\n");
    pthread_mutex_unlock(&print_mutex);

    /* Detach and remove shared memory */
    shmdt(seats);
    shmctl(shmid, IPC_RMID, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&print_mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
