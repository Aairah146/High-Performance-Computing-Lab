#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

#define SIZE 5
#define MAX_JOBS 20  // Stop after 20 jobs

// Job structure
typedef struct {
    int a;
    int b;
    char operator;
    int job_id;
} Job;

Job queue[SIZE];
int front = 0;
int rear = 0;
int id_counter = 1;
int job_count = 0;
int total_jobs_created = 0;  // Track total jobs created
int keep_running = 1;        // Flag to control running

sem_t slots_available;
sem_t jobs_available;
sem_t lock;
pthread_mutex_t file_lock;
FILE *output;

// Function to execute job
double execute(Job j) {
    if (j.operator == '+') return j.a + j.b;
    if (j.operator == '-') return j.a - j.b;
    if (j.operator == '*') return j.a * j.b;
    if (j.operator == '/' && j.b != 0) return (double)j.a / j.b;
    return 0;
}

// Helper function to check if queue is full
int isFull() {
    return job_count == SIZE;
}

// Helper function to check if queue is empty
int isEmpty() {
    return job_count == 0;
}

// Signal handler for Ctrl+C
void signal_handler(int sig) {
    keep_running = 0;
    printf("\nStopping program...\n");
}

// Producer Function
void* server(void* arg) {
    char symbols[] = {'+', '-', '*', '/'};
    
    while (keep_running) {
        // Check if we've created enough jobs
        if (total_jobs_created >= MAX_JOBS) {
            printf("Producer: Max jobs reached. Stopping.\n");
            break;
        }
        
        Job newJob;
        newJob.a = rand() % 50 + 1;
        newJob.b = rand() % 50 + 1;
        newJob.operator = symbols[rand() % 4];
        newJob.job_id = id_counter++;
        total_jobs_created++;
        
        // Wait for empty slot
        sem_wait(&slots_available);
        
        // Lock for queue access
        sem_wait(&lock);
        
        // EXPLICIT CHECK: Queue full?
        if (isFull()) {
            printf("Queue is FULL! Cannot add job.\n");
            sem_post(&lock);
            sem_post(&slots_available);
            sleep(1);
            continue;
        }
        
        // Add job to queue
        queue[rear] = newJob;
        rear = (rear + 1) % SIZE;
        job_count++;
        
        printf("New Job Created [ID:%d] -> %d %c %d | Jobs in queue: %d\n",
               newJob.job_id, newJob.a, newJob.operator, newJob.b, job_count);
        
        sem_post(&lock);
        sem_post(&jobs_available);
        
        sleep(1);
    }
    
    // Signal workers that no more jobs coming
    // Post remaining jobs_available so workers can exit
    int i;
    for ( i = 0; i < 4; i++) {
        sem_post(&jobs_available);
    }
    
    pthread_exit(NULL);
}

// Consumer Function
void* worker(void* arg) {
    char mySymbol = *(char*)arg;
    
    while (keep_running) {
        // Wait for at least one job (with timeout check)
        int ret = sem_trywait(&jobs_available);
        if (ret != 0) {
            // No job available, check if we should exit
            if (total_jobs_created >= MAX_JOBS) {
                break;
            }
            usleep(100000);  // Wait 0.1 second
            continue;
        }
        
        // Lock for queue access
        sem_wait(&lock);
        
        // EXPLICIT CHECK: Queue empty?
        if (isEmpty()) {
            sem_post(&lock);
            sem_post(&jobs_available);  // Put the job token back
            continue;
        }
        
        // Get job from queue
        Job current = queue[front];
        front = (front + 1) % SIZE;
        job_count--;
        
        printf("Worker [%c] picked Job %d | Jobs in queue: %d\n",
               mySymbol, current.job_id, job_count);
        
        sem_post(&lock);
        sem_post(&slots_available);
        
        // Process only matching operators
        if (current.operator == mySymbol) {
            double answer = execute(current);
            
            // Lock for file writing (critical section)
            pthread_mutex_lock(&file_lock);
            fprintf(output,
                    "Job %d: %d %c %d = %.2lf\n",
                    current.job_id,
                    current.a,
                    current.operator,
                    current.b,
                    answer);
            fflush(output);  // Ensure data is written immediately
            pthread_mutex_unlock(&file_lock);
            
            printf("Worker [%c] completed Job %d -> Result: %.2lf\n",
                   mySymbol,
                   current.job_id,
                   answer);
        }
    }
    
    printf("Worker [%c] exiting.\n", mySymbol);
    pthread_exit(NULL);
}

int main() {
    srand(time(NULL));
    
    // Set up signal handler for Ctrl+C
    signal(SIGINT, signal_handler);
    
    pthread_t producer_thread;
    pthread_t t1, t2, t3, t4;
    
    char plus = '+';
    char minus = '-';
    char multiply = '*';
    char divide = '/';
    
    output = fopen("input.txt", "w");
    if (output == NULL) {
        printf("Error opening file!\n");
        return 1;
    }
    
    sem_init(&slots_available, 0, SIZE);
    sem_init(&jobs_available, 0, 0);
    sem_init(&lock, 0, 1);
    pthread_mutex_init(&file_lock, NULL);
    
    pthread_create(&producer_thread, NULL, server, NULL);
    pthread_create(&t1, NULL, worker, &plus);
    pthread_create(&t2, NULL, worker, &minus);
    pthread_create(&t3, NULL, worker, &multiply);
    pthread_create(&t4, NULL, worker, &divide);
    
    pthread_join(producer_thread, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);
    
    fclose(output);
    
    sem_destroy(&slots_available);
    sem_destroy(&jobs_available);
    sem_destroy(&lock);
    pthread_mutex_destroy(&file_lock);
    
    printf("\nProgram completed! Check input.txt for results.\n");
    
    return 0;
}
