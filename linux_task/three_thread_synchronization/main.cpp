
// C program to implement cond(), signal()
// and wait() functions
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
 
// Declaration of thread condition variable
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond3 = PTHREAD_COND_INITIALIZER;
 
// declaring mutex
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
 
int global_var = 1;
 
// Thread function
void* t1(void *arg)
{
    // acquire a lock
    int i=1;
    for(int j=0;j<10;j++)
    {
        pthread_mutex_lock(&lock);
        printf("T1 %d \n",global_var++);
        pthread_cond_signal(&cond2);
        pthread_cond_wait(&cond1,&lock);
        pthread_mutex_unlock(&lock);
    }       
    pthread_cond_signal(&cond1);
     pthread_cond_signal(&cond2);
     pthread_cond_signal(&cond3);
    printf("T1 return\n");
    return NULL;
}
void* t2(void *arg)
{
    // acquire a lock
    int i=1;
    for(int j=0;j<10;j++)
    {
        pthread_mutex_lock(&lock);
        printf("T2 %d \n",global_var++);
        pthread_cond_signal(&cond3);
        pthread_cond_wait(&cond2,&lock);
        pthread_mutex_unlock(&lock);
    }
    pthread_cond_signal(&cond1);
     pthread_cond_signal(&cond2);
     pthread_cond_signal(&cond3);
    printf("T2 return\n");
    return NULL;
}

void* t3(void *arg)
{
    // acquire a lock
    int i=1;
    for(int j=0;j<10;j++)
    {
        pthread_mutex_lock(&lock);
        printf("T3 %d \n",global_var++);
        pthread_cond_signal(&cond1);
        pthread_cond_wait(&cond3,&lock);
        pthread_mutex_unlock(&lock);
    }    
     pthread_cond_signal(&cond1);
     pthread_cond_signal(&cond2);
     pthread_cond_signal(&cond3);
    printf("T3 return \n");
    return NULL;
}

int main()
{
    pthread_t tid1, tid2,tid3;
    pthread_create(&tid1, NULL, t1, NULL);
    usleep(100);
    pthread_create(&tid2, NULL, t2, NULL);
    usleep(100);
    pthread_create(&tid3, NULL, t3, NULL);
 
    // wait for the completion of thread 2
    pthread_join(tid3, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid1, NULL);
 
    return 0;
}

