#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <math.h>
#define TB "\033[0;34m"
#define TRST "\033[0m"
#define TR "\033[0;31m"
#define TG "\033[0;32m"
#define TB "\033[0;34m"
#define TM "\033[0;35m"
// inline void terminal_red(void){
//     printf("\033[0;31m");
// }
// inline void terminal_reset(void){
//     printf("\033[0m");
// }

#define READERS_NUM 6
#define WRITERS_NUM 8
#define SWRITER_SIZE 3
#define BOOKS 20
#define QUEUE_ELEMENTS 15
#define QUEUE_SIZE (QUEUE_ELEMENTS + 1)
char Queue[QUEUE_SIZE];
int QueueIn, QueueOut, QueueLen;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

struct library
{
    char books[BOOKS];
    int top;
} libb;
#define REVIER_REQUIRED 1
#define REVIER_NOT_REQUIRED 2
int reviewer_state = 2;

sem_t readerSem;
sem_t writerSem;

int readers_count = 0;
void writer(int *i)
{
    int id = *i;
    printf("%sPisarz %d probuje pisac....%s\r\n", TR, id, TRST);
    //pisanie
    pthread_mutex_lock(&mutex);
    while (reviewer_state == REVIER_REQUIRED)
    {
        pthread_cond_wait(&cond, &mutex);
    }
    printf("%sPisarz %d Pisze....%c\r\n%s", TR, id, (char)(id + 65) % 122, TRST);

    libb.books[libb.top] = (char)(id + 65) % 122;
    libb.top++;
    reviewer_state = REVIER_REQUIRED;
    pthread_cond_broadcast(&cond);
    printf("%sPisarz %d zapisalem....\r\n%s", TR, id, TRST);

    pthread_mutex_unlock(&mutex);
}
void reviewer(int *i)
{
    while (1)
    {
        pthread_mutex_lock(&mutex);
        while (reviewer_state == REVIER_NOT_REQUIRED)
        {
            pthread_cond_wait(&cond, &mutex);
        }
        printf("%sKrytyk Pisze....%c\r\n%s", TM, 'R', TRST);
        libb.books[libb.top] = 'R';
        libb.top++;
        reviewer_state = REVIER_NOT_REQUIRED;
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mutex);
    }
}

void reader(int *i)
{
    int id = *i;
    printf("%sCzytelnik %d probuje zaczac czytac....\r\n%s", TG, id, TRST);
    sem_wait(&readerSem);
    readers_count++;
    if (readers_count == 1)
    {
        pthread_mutex_lock(&mutex);
    }
    sem_post(&readerSem);
    printf("%sCzytelnik %d czytam....%s%s\r\n", TG, id, libb.books, TRST);
    //czytania
    sem_wait(&readerSem);
    readers_count--;
    if (readers_count == 0)
    {
        pthread_mutex_unlock(&mutex);
    }
    printf("%sCzytelnik %d przeczytalem....%s\r\n", TG, id, TRST);

    sem_post(&readerSem);
}

int main()
{
    libb.top = 0;
    srand((unsigned)time(NULL));
    sem_init(&readerSem, 0, 1);
    sem_init(&writerSem, 0, 1);
    for (int i = 0; i < BOOKS; i++)
        libb.books[i] = '\0';
    pthread_t readers[READERS_NUM];
    pthread_t writers[WRITERS_NUM];
    pthread_t reviewer_id;
    int swriter_id = 0;

    int reader_ids[READERS_NUM];
    int writer_ids[WRITERS_NUM];
    if (pthread_create(&(reviewer_id), NULL, (void *(*)(void *)) & reviewer, &swriter_id) != 0)
    {
        perror("error");
    }
    for (int i = 0; i < WRITERS_NUM; i++)
    {
        writer_ids[i] = i;
        if (pthread_create(&(writers[i]), NULL, (void *(*)(void *)) & writer, &(writer_ids[i])) != 0)
        {
            perror("error");
        }
    }

    for (int i = 0; i < READERS_NUM; i++)
    {
        reader_ids[i] = i;
        if (pthread_create(&(readers[i]), NULL, (void *(*)(void *)) & reader, &(reader_ids[i])) != 0)
        {
            perror("error");
        }
    }

    for (int i = 0; i < WRITERS_NUM; i++)
    {
        pthread_join((writers[i]), NULL);
    }

    for (int i = 0; i < READERS_NUM; i++)
    {
        pthread_join((readers[i]), NULL);
    }

    pthread_join(reviewer_id, NULL);
}