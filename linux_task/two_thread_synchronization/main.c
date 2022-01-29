
// C program to implement cond(), signal()
// and wait() functions
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
 
// Declaration of thread condition variable
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
 
// declaring mutex
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
 
int done = 1;
 
// Thread function
void* foo1()
{
 
    // acquire a lock
    int i=1;
    for(int j=0;j<10;j++)
    {
        pthread_mutex_lock(&lock);
        printf(" %d \n",i);
        i+=2;
        pthread_cond_signal(&cond1);
        pthread_cond_wait(&cond1, &lock);
        pthread_mutex_unlock(&lock);
    }       
     pthread_cond_signal(&cond1);
    return NULL;
}
 void* foo2()
{
 
    // acquire a lock
    int i=2;
    for(int j=0;j<10;j++)
    {
        pthread_mutex_lock(&lock);
        printf(" %d \n",i);
        i+=2;
        pthread_cond_signal(&cond1);
        pthread_cond_wait(&cond1, &lock);

       pthread_mutex_unlock(&lock);
        //sleep(1);
    }
   pthread_cond_signal(&cond1);
    // release lock

 
    printf("Returning thread\n");
 
    return NULL;
}
// Driver code
int main()
{
    pthread_t tid1, tid2;
 
    // Create thread 1
    pthread_create(&tid1, NULL, foo1, NULL);
    // Create thread 2
    usleep(100);
    pthread_create(&tid2, NULL, foo2, NULL);
 
    // wait for the completion of thread 2
    pthread_join(tid2, NULL);
    pthread_join(tid1, NULL);
 
    return 0;
}
