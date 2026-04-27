#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX 1000

int arr[MAX];
int n, i, thread_count;
long global_sum = 0;
long thread;
pthread_mutex_t mutex;

void* partial_sum(void* rank) {
    long my_rank = (long)rank;
    
    int local_n = n / thread_count;
    int first = my_rank * local_n;
    int last = (my_rank == thread_count - 1) ? n - 1 : first + local_n - 1;
    
    long local_sum = 0;
    
    printf("Thread %ld: First index=%d, Last index=%d\n", my_rank, first, last);
    
    for (i = first; i <= last; i++)
        local_sum += arr[i];
    
    pthread_mutex_lock(&mutex);
    global_sum += local_sum;
    pthread_mutex_unlock(&mutex);
    
    pthread_exit(NULL);
}

int main() {
    pthread_t* thread_handles;
    pthread_mutex_init(&mutex, NULL);
    
    printf("Enter number of elements: ");
    scanf("%d", &n);
    
    // Generate random elements
    srand(time(NULL));
    printf("Generated elements: ");
    for (i = 0; i < n; i++) {
        arr[i] = rand() % 100 + 1;  // Random numbers between 1 and 100
        printf("%d ", arr[i]);
    }
    printf("\n");
    
    printf("Enter number of threads: ");
    scanf("%d", &thread_count);
    
    thread_handles = malloc(thread_count * sizeof(pthread_t));
    
    for (thread = 0; thread < thread_count; thread++)
        pthread_create(&thread_handles[thread], NULL, partial_sum, (void*)thread);
    
    for (thread = 0; thread < thread_count; thread++)
        pthread_join(thread_handles[thread], NULL);
    
    printf("\nTotal Sum = %ld\n", global_sum);
    
    pthread_mutex_destroy(&mutex);
    free(thread_handles);
    
    return 0;
}
