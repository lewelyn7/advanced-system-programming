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

#define LIBS_NUM 2



struct library
{
    char books[BOOKS];
    int top;
    pthread_mutex_t r_mutex;
    pthread_mutex_t w_mutex;
    int readers_count;
};
struct library libs[LIBS_NUM];


sem_t readerSem;
sem_t writerSem;
sem_t bufferSem;

int readers_count = 0;
void writer(int *i)
{
    int id = *i;
    int it = 0;
    printf("%sPisarz %d probuje pisac....%s\r\n",TR, id,TRST);
    while(pthread_mutex_trylock(&libs[it].w_mutex) != 0){
        it++;
        if(it == LIBS_NUM) it = 0;
    }    
    sleep(1);
    printf("%sPisarz %d Pisze do %d ....%c\r\n%s",TR, id, it, (char) (id+65)%122, TRST);
    libs[it].books[libs[it].top] = (char) (id+65)%122;
    libs[it].top++;
    printf("%sPisarz %d zapisalem....\r\n%s",TR, id, TRST);
    pthread_mutex_unlock(&libs[it].w_mutex);
}



void reader(int *i)
{
    int id = *i;
    printf("%sCzytelnik %d probuje zaczac czytac....\r\n%s",TG, id, TRST);
    int it = 0;
    while(pthread_mutex_trylock(&libs[it].r_mutex) != 0){
        it++;
        if(it == LIBS_NUM) it  = 0;
    }
    libs[it].readers_count++;
    if (readers_count == 1)
    {
        pthread_mutex_lock(&libs[it].w_mutex);
    }
        sleep(1);
    pthread_mutex_unlock(&libs[it].r_mutex);
    printf("%sCzytelnik %d czytam z %d....%s%s\r\n",TG, id, it, libs[it].books ,TRST);
    //czytania
    pthread_mutex_lock(&libs[it].r_mutex);
    libs[it].readers_count--;
    if (libs[it].readers_count == 0)
    {
        pthread_mutex_unlock(&libs[it].w_mutex);
    }
        printf("%sCzytelnik %d przeczytalem....%s\r\n",TG, id, TRST);

    pthread_mutex_unlock(&libs[it].r_mutex);
}
void libs_init(void){
    for(int i = 0; i < LIBS_NUM; i++){
        libs[i].top = 0;
        for(int j = 0; j < BOOKS; j++) libs[i].books[j] = '\0';
        pthread_mutex_init(&(libs[i].r_mutex), NULL);
        pthread_mutex_init(&(libs[i].w_mutex), NULL);
        readers_count = 0;
    }
}
int main()
{

    libs_init();
    srand((unsigned)time(NULL));
    sem_init(&readerSem, 0, 1);
    sem_init(&writerSem, 0, 1);
    sem_init(&bufferSem, 0, 1);
    pthread_t readers[READERS_NUM];
    pthread_t writers[WRITERS_NUM];
    pthread_t swriter;
    int swriter_id = 0;
    int reader_ids[READERS_NUM];
    int writer_ids[WRITERS_NUM];
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
}