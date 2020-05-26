
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<pthread.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<linux/ioctl.h>
#include<sched.h>
#include<time.h>
#include<string.h>

pthread_attr_t attr,attr2;
pthread_t t1,t2,t3,t4;

struct sched_param p1,p2,p;

unsigned int global;

void *w1()
{
int x=10000;
//printf("TID1=%d",gettid());
printf("Add=%p\n",&x);
printf("Initial Value x=%d\n",global);

while(x--)
{
	global++;
}
printf("T1 Final Value x=%d\n",global);
pthread_exit(0);

}

void *w2()
{
int x=10000;
//printf("TID2=%d",gettid());
printf("Add=%p\n",&x);
printf("Initial Value x=%d\n",global);
while(x--)
{
	global++;
}
printf("T2 Final Value x=%d\n",global);
pthread_exit(0);		//terminate calling thread

}

void main()
{

int ret;

printf("Thread Program\n");
printf("Main PID=%d\n",getpid());
int x=2;

printf("Main=%p\n",&x);
ret=pthread_attr_init(&attr);		         //initialize thread object
ret=pthread_attr_init(&attr2);		         //initialize thread object

if(ret!=0)
{
	perror("Error in creating Object");
	exit(1);
}

// pthread_attr_setschedpolicy(&attr,SCHED_FIFO);   //Setting Shedule Policy of thread 							// Attribute object		



pthread_attr_setinheritsched(&attr,PTHREAD_INHERIT_SCHED);

ret= pthread_create(&t1,&attr,w1,NULL);
if(ret==0){printf("Thread1 created sucessfully\n");}

//p.sched_priority=99;
//pthread_setschedparam(pthread_self(), SCHED_FIFO, &p);


p2.sched_priority=95;
pthread_attr_setschedpolicy(&attr2,SCHED_FIFO);

pthread_attr_setschedparam(&attr2,&p2);
pthread_attr_setinheritsched(&attr2,PTHREAD_EXPLICIT_SCHED);
ret=pthread_create(&t2,&attr2,w2,NULL);

if(ret==0){printf("Thread2 created sucessfully\n");}
else
printf("thread creation unsuccessful\n");
printf("Main Works Done>>>\n");

pthread_join(t1,NULL);
pthread_join(t2,NULL);
	
	//while(1);
}
