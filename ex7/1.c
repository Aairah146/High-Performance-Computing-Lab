#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#define BUFFER_SIZE 5
int TOTAL_TASKS;
typedef struct {
    int num1;
    int num2;
    char op;
} task;
task buffer[BUFFER_SIZE];
int in = 0, out = 0;
sem_t empty;
sem_t full;
pthread_mutex_t mutex;
FILE *fp;
void insert_task(task t) {
    buffer[in] = t;
    in = (in + 1) % BUFFER_SIZE;
}
task remove_task() {
    task t = buffer[out];
    out = (out + 1) % BUFFER_SIZE;
    return t;
}
/* Worker Thread */
void* worker(void *arg) {
    char operation = *(char*)arg;
    while (1) {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        task t = remove_task();
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
        if (t.op == 'X')
            break;
        if (t.op != operation) {
            sem_wait(&empty);
            pthread_mutex_lock(&mutex);
            insert_task(t);
            pthread_mutex_unlock(&mutex);
            sem_post(&full);
            usleep(100000);
            continue;
        }
        float result;
        switch (operation) {
            case '+': result = t.num1 + t.num2; break;
            case '-': result = t.num1 - t.num2; break;
            case '*': result = t.num1 * t.num2; break;
            case '/':
                if (t.num2 != 0)
                    result = (float)t.num1 / t.num2;
                else
                    result = 0;
                break;
        }
        pthread_mutex_lock(&mutex);
        fprintf(fp, "[%c] %d %c %d = %.2f\n",
                operation, t.num1, operation, t.num2, result);
        fflush(fp);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}
/* Server Thread */
void* server(void *arg) {
    char ops[] = {'+', '-', '*', '/'};
    srand(time(NULL));
    int i;
    for (i = 0; i < TOTAL_TASKS; i++) {
        task t;
        t.num1 = rand() % 100;
        t.num2 = rand() % 50 + 1;
        t.op = ops[rand() % 4];
        printf("[SERVER] Task %d: %d %c %d\n", i+1, t.num1, t.op, t.num2);
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        insert_task(t);
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
        sleep(1);
    }
    int j;
    for (j = 0; j < 4; j++) {
        task t;
        t.op = 'X';
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        insert_task(t);
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
    return NULL;
}
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <total_tasks>\n", argv[0]);
        return 1;
    }
    TOTAL_TASKS = atoi(argv[1]);
    if (TOTAL_TASKS <= 0) {
        printf("Enter a positive number\n");
        return 1;
    }
    pthread_t server_thread;
    pthread_t workers[4];
    char ops[4] = {'+', '-', '*', '/'};
    fp = fopen("result.txt", "w");
    if (fp == NULL) {
        printf("Error opening file\n");
        return 1;
    }
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    pthread_mutex_init(&mutex, NULL);
    int k;
    for (k = 0; k < 4; k++) {
        pthread_create(&workers[k], NULL, worker, &ops[k]);
    }
    pthread_create(&server_thread, NULL, server, NULL);
    pthread_join(server_thread, NULL);
    int m;
    for (m = 0; m < 4; m++) {
        pthread_join(workers[m], NULL);
    }
    fclose(fp);
    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);
    printf("\nCalculation completed. Check result.txt\n");
    return 0;
}
