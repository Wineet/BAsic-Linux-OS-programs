/*
 * Author: Vinit
 * Platform: Linux
 * Hardwar: R-pi 3B+
 * Task :Create a software timer with Call back (signal )
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


void timer_exp_handler(int ,siginfo_t *,void *);

void main()
{
	timer_t timerid;
	struct sigevent segv;
	struct itimerspec timer_spec;
	struct sigaction sig_action;
	pthread_attr_t thread_attr;


	if(0 != pthread_attr_init(&thread_attr))
	{
		printf("warn:Pthread_attr_init Failed\n");
	}	
	if(0!= pthread_attr_setdetachstate(&thread_attr,PTHREAD_CREATE_DETACHED))
	{
		printf("warn: Pthread detached Failed\n");
	}	


	sig_action.sa_flags = SA_SIGINFO;
	sig_action.sa_sigaction = timer_exp_handler;
	
	if( 0!= sigaction(SIGRTMIN,&sig_action,NULL))			//Only SIGKILL and SIGSTOP are invalid
	{
		printf("warn: Sigaction Failed");
	
	}

	//Timer Init
	segv.sigev_notify = SIGEV_SIGNAL;
	segv.sigev_signo = SIGRTMIN;
	segv.sigev_value.sival_ptr = &timerid ;


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



void timer_exp_handler(int sig_num, siginfo_t *sig_info,void * dummy2)
{
	
	/*Printf Should not be used in production programms
	 * Since printf is not asynch-signal safe */

	printf("Timer Expiery Handler signal %d\n",sig_num);

	timer_t  *tim_id =(timer_t *) sig_info->si_value.sival_ptr;
	printf("Destory timer Id =%x\n",*tim_id);

	if(0 != timer_delete( *tim_id)) 
	{
		printf("Timer Destory Error");
	}
	for(int i=0;i<5;i++)
	{
		printf("I in Handler\n");
		sleep(1);
	
	}

}


