
// C program to implement cond(), signal()
// and wait() functions
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
 
//declaring semaphore 
sem_t sema1;
sem_t sema2;
sem_t sema3;

void* t1(void *arg)
{
    for(int j=0;j<10;j++)
    {
        sem_wait(&sema1);
        printf("Thread 1 \n");
        sem_post(&sema2);
    }       
    printf("T1 return\n");
    return NULL;
}
void* t2(void *arg)
{
    for(int j=0;j<10;j++)
    {
        sem_wait(&sema2);
        printf("Thread 2 \n");
        sem_post(&sema3);
    }
    printf("T2 return\n");
    return NULL;
}

void* t3(void *arg)
{
    for(int j=0;j<10;j++)
    {
        sem_wait(&sema3);
        printf("Thread 3 \n");
        sem_post(&sema1);
    }    
    printf("T3 return \n");
    return NULL;
}

int main()
{
    pthread_t tid1, tid2,tid3;
    sem_init(&sema1,0,1);
    sem_init(&sema2,0,0);
    sem_init(&sema3,0,0);
    pthread_create(&tid1, NULL, t1, NULL);
    usleep(100);
    pthread_create(&tid2, NULL, t2, NULL);
    usleep(100);
    pthread_create(&tid3, NULL, t3, NULL);

    pthread_join(tid3, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid1, NULL);
    sem_destroy(&sema1);
    sem_destroy(&sema2);
    sem_destroy(&sema3);
    return 0;
}

