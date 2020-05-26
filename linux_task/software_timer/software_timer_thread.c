/*
 * Author: Vinit
 * Platform: Linux
 * Hardwar: R-pi 3B+
 * Task :Create a software timer with Call back (call Back is handled by Thread / a newthread will be created)
 *
 * For Compilation please attach >> -pthread -lrt << liberaries
 */


#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<signal.h>
#include<time.h>
#include<pthread.h>
#include<unistd.h>


void timer_exp_handler(union sigval);

void main()
{
	timer_t timerid;
	struct sigevent segv;
	struct itimerspec timer_spec;

	pthread_attr_t thread_attr;

	if(0 != pthread_attr_init(&thread_attr))
	{
		printf("warn:Pthread_attr_init Failed\n");
	}
	if(0!= pthread_attr_setdetachstate(&thread_attr,PTHREAD_CREATE_DETACHED))	//Set thread in detached state
	// Timer Initilised
		
	{
		printf("warn: pthread Detached Error\n");
	
	}
	segv.sigev_notify = SIGEV_THREAD;
	segv.sigev_value.sival_ptr = &timerid ;
	segv.sigev_notify_function = &timer_exp_handler;
	segv.sigev_notify_attributes = &thread_attr;

	if(0 != timer_create(CLOCK_REALTIME,&segv,&timerid))
	{
		printf("warn: timer Create Failed\n");
	}
	
	printf("Timer Id =%x\n",timerid);
	//Start Timer 
	timer_spec.it_interval.tv_sec = 5;
	timer_spec.it_interval.tv_nsec= 0;
	timer_spec.it_value.tv_sec = 5;
	timer_spec.it_value.tv_nsec = 0;

	if(0 != timer_settime(timerid,0,&timer_spec,NULL))
	{
		printf("Timer set time Failed\n");
	
	}
	printf("Waiting for timer Expiery\n");

	for(int i=0;i<10;i++)
	{
		printf("I am Main\n");
		sleep(1);
	
	}

	return;
}



void timer_exp_handler(union sigval sig)
{

	printf("Timer Expiery Handler\n");
	timer_t *tim_id =(timer_t *)sig.sival_ptr;
	printf("Destory timer Id =%x\n",*tim_id);

	if(0 != timer_delete( *tim_id)) 
	{
		printf("Timer Destory Error");
	}
	for(int i=0;i<5;i++)
	{
		printf("I am Thread\n");
		sleep(1);
	
	}
	exit(EXIT_SUCCESS);

}


