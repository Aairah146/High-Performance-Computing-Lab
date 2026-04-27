#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#define BUFFER_SIZE 50
#define WORD_LEN 20

char *dictionary[] = {
    "apple","banana","cat","dog","computer",
    "science","book","program","server","thread"
};
int dict_size = 10;
char work_buffer[BUFFER_SIZE][WORD_LEN];
int work_count = 0;
char result_buffer[BUFFER_SIZE][50];
int result_count = 0;
pthread_mutex_t mutex;
pthread_cond_t cond;
int total_words;

int check_dictionary(char word[])
{
    int i;
    for(i=0;i<dict_size;i++)
    {
        if(strcmp(word,dictionary[i])==0)
            return 1;
    }
    return 0;
}

void* spell_check(void* arg)
{
    int thread_id = *(int*)arg;
    while(1)
    {
        pthread_mutex_lock(&mutex);
        while(work_count == 0)
            pthread_cond_wait(&cond,&mutex);
        char word[WORD_LEN];
        strcpy(word,work_buffer[--work_count]);
        printf("[T%d] Processing: %s\n", thread_id, word);
        fflush(stdout);
        pthread_mutex_unlock(&mutex);

        int correct = check_dictionary(word);

        pthread_mutex_lock(&mutex);
        if(correct)
            sprintf(result_buffer[result_count++],"[T%d] [OK] %s",thread_id,word);
        else
            sprintf(result_buffer[result_count++],"[T%d] [MISS] %s",thread_id,word);
        pthread_mutex_unlock(&mutex);
    }
}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        printf("Usage: %s <number_of_threads>\n",argv[0]);
        return 1;
    }
    int THREADS = atoi(argv[1]);

    printf("Dictionary Words:\n");
    int i;
    for(i=0;i<dict_size;i++)
        printf("%s ",dictionary[i]);
    printf("\n\n");

    printf("Enter number of words to check: ");
    scanf("%d",&total_words);

    char input_words[BUFFER_SIZE][WORD_LEN];
    int j;
    for(j=0;j<total_words;j++)
    {
        printf("Enter word %d: ",j+1);
        scanf("%s",input_words[j]);
    }

    pthread_t threads[THREADS];
    int thread_ids[THREADS];

    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&cond,NULL);

    int k;
    for(k=0;k<THREADS;k++)
    {
        thread_ids[k] = k + 1;
        pthread_create(&threads[k],NULL,spell_check,&thread_ids[k]);
    }

    int m;
    for(m=0;m<total_words;m++)
    {
        pthread_mutex_lock(&mutex);
        strcpy(work_buffer[work_count++],input_words[m]);
        printf("[SERVER] Queued: %s\n",input_words[m]);
        fflush(stdout);
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }

    sleep(2);

    printf("\n=== Spell Check Results ===\n");
    int n;
    for(n=0;n<result_count;n++)
        printf("%s\n",result_buffer[n]);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
