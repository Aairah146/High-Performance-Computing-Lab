#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#define MAX_LINES 20
#define MAX_LEN 200
char queue[MAX_LINES][MAX_LEN];
int front = 0, rear = 0;
int total_tokens = 0;
int producer_done = 0;
omp_lock_t lock;
int main()
{
    FILE *fp = fopen("data.txt", "r");
    if (fp == NULL)
    {
        printf("File not found!\n");
        return 1;
    }
    omp_set_num_threads(4);
    omp_init_lock(&lock);
    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        if (id % 2 == 0)  // Producer threads
        {
            int count = 0;
            while (count < MAX_LINES/2)
            {
                char line[MAX_LEN] = "";
                int got_line = 0;
                #pragma omp critical
                {
                    if (fgets(line, MAX_LEN, fp) != NULL)
                        got_line = 1;
                    else
                        count = MAX_LINES/2; 
                }
                if (got_line)
                {
                    omp_set_lock(&lock);
                    if (rear < MAX_LINES)
                    {
                        strcpy(queue[rear++], line);
                    }
                    omp_unset_lock(&lock);
                    count++;
                }
            }
            #pragma omp atomic
            producer_done++;
        }
        else  // Consumer threads
        {
            while (1) 
            {
                char line[MAX_LEN] = "";
                omp_set_lock(&lock);
                if (front < rear)
                    strcpy(line, queue[front++]);
                omp_unset_lock(&lock);
                if (strlen(line) > 0)
                {
                    char *saveptr;
                    char *token = strtok_r(line, " \n", &saveptr);
                    while (token != NULL)
                    {
                        #pragma omp critical
                        printf("Thread %d | Token: %s\n", id, token);
                        #pragma omp atomic
                        total_tokens++;
                        token = strtok_r(NULL, " \n", &saveptr);
                    }
                }
                else
                {
                    
                    if (producer_done >= 2 && front >= rear)
                        break;
                }
            }
        }
    }
    printf("\nTotal Tokens = %d\n", total_tokens);
    fclose(fp);
    omp_destroy_lock(&lock);
    return 0;
}
