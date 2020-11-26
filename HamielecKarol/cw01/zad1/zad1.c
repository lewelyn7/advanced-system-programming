#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>


#define READERS_NUM 6
#define WRITERS_NUM 3


sem_t readerSem;
sem_t writerSem;
int readers_count = 0;
void writer(int * i){
    int id = *i;
    printf("Pisarz %d probuje pisac....\r\n", id);
    sem_wait(&writerSem);
    //pisanie
    printf("Pisarz %d Pisze....\r\n", id);

    sem_post(&writerSem);

    printf("Pisarz %d zapisalem....\r\n", id);
}

void reader(int * i){
    int id = *i;
    printf("Czytelnik %d probuje zaczac czytac....\r\n", id);
    sem_wait(&readerSem);
    readers_count++;
    if(readers_count == 1){
        sem_wait(&writerSem);
    }
    sem_post(&readerSem);
    printf("Czytelnik %d czytam....\r\n", id);
    //czytania
    sem_wait(&readerSem);
    readers_count--;
    if(readers_count == 0){
        sem_post(&writerSem);
    }
    sem_post(&readerSem);
    printf("Czytelnik %d przeczytalem....\r\n", id);
}

int main(){

    sem_init(&readerSem, 0, 1);
    sem_init(&writerSem, 0, 1);

    pthread_t readers[READERS_NUM];
    pthread_t writers[WRITERS_NUM];
    int reader_ids[READERS_NUM];
    int writer_ids[WRITERS_NUM];

    for(int i = 0; i < WRITERS_NUM; i++){
        writer_ids[i] = i;
        if(pthread_create(&(writers[i]), NULL,(void * (*)(void *)) &writer, &(writer_ids[i])) != 0){
            perror("error");
        }
    }

    for(int i = 0; i < READERS_NUM; i++){
        reader_ids[i] = i;
        if(pthread_create(&(readers[i]), NULL, (void * (*)(void *)) &reader, &(reader_ids[i])) != 0){
            perror("error");
        }        
    }

    for(int i = 0; i < WRITERS_NUM; i++){
        pthread_join((writers[i]),NULL); 
    }

    for(int i = 0; i < READERS_NUM; i++){
        pthread_join((readers[i]),NULL); 
    }

}