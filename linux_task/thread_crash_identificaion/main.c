/*
 * Author:   vinit 
 * Task:     Detect Crash Thread
 * Hw  : 	   Linux ubuntu 20.0+
 * Platform: Linux
 *
 */


#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<pthread.h>
#include<signal.h>

#define NUMBER_OF_THREADS 3

void sig_sev_signal_handler(int hand_arg);
void *common_thread_routine(void *arg);

int var =1;
pthread_t thread_id[NUMBER_OF_THREADS]={0};
pthread_t failed_thread=0;
void common_fun(char *ptr,int size);
int  main(int argc,char *argv[])
{
   
    for(int i=0;i<NUMBER_OF_THREADS;i++)
    {
        int ret=0;
        ret=pthread_create(thread_id+i,NULL,common_thread_routine,(void *)thread_id+i);
        if(ret!=0)
        {
            printf("Pthread_create Failed %s\n",strerror(errno));
            return -1;
        }
        sleep(1);
    }
    signal(SIGINT,sig_sev_signal_handler);
    for(int i=0;i<NUMBER_OF_THREADS;i++)
    {
        pthread_join(thread_id[i],NULL);
    }
    return 0;
}
void *common_thread_routine(void *arg)
{
    while(1)
    {
        printf("Thread %ld var %d\n",(long int)((long int *)arg),var);
        char *ptr=malloc(1024*1);   //Malloc Failure Case Not handled
        common_fun(ptr,1024);
        sleep(1);
    }

}

void sig_sev_signal_handler(int hand_arg)
{
    printf("SIG_int Handler \n");
    for(int i=0;i<NUMBER_OF_THREADS;i++)
    {
       if( 0 != pthread_cancel(thread_id[i]))
       {
           failed_thread=thread_id[i];
           printf("Thread_id Not present %ld",thread_id[i]);
       }
        var++;
    }
}

//writing to memory Locations
void common_fun(char *ptr,int size)
{
    memset(ptr,1,size);
}