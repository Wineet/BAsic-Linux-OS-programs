/*
 * Author:   vinit 
 * Task:     process Reboot Software reset Without changing Process attribute
 * Hw  : 	   R-pi 3B+
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

/*Added Header Files Below*/

#include<pthread.h>

#define DEBUG 1
 
#if DEBUG == 1
 		#define dbug(arg,...) printf(arg,##__VA_ARGS__)
#else
 		#define dbug(arg,...)  
#endif


/*Function Declaration Here*/

void * thread_routine(void * arg);
void *process_reboot(void *arg);

/*Global Variable Here*/

static	pthread_t thread1,thread2,thread3;

int  main(int argc,char *argv[])
{


/* Creating Two Demo thread
 * which will print Periodic 
 * Process Reboot Function is doing actual reboot
 *
 * */
	printf("My Procces Id %ld\n",getpid());
pthread_attr_t thread_attr;

	if( 0 != pthread_attr_init(&thread_attr))
	{
		printf("attrobute Failed\n");
	}

	/*
	 * Thread create Exception Not Handled
	 *
	 * */

	pthread_create(&thread1,NULL,thread_routine,"thread 1");
	pthread_create(&thread2,NULL,thread_routine,"thread 2");
	pthread_create(&thread3,NULL,process_reboot,"ProcessReboot");
	
	printf("T1=%ld T2=%ld T3=%ld \n",thread1,thread2,thread3);

//	pthread_exit(0);

	while(1)
	{
		sleep(1);
	}

	/*
	 * process has executable file name as
	 *
	 * process_rebooting so by doing execl we will load same process again
	 * Complete Memory Map will be written again but PID will be same
	 *
	 * */


return 0;
}

void *process_reboot(void *arg)
{
	char *t_name = (char *)arg;

	 if(0 != pthread_detach(pthread_self()))
	 {
	 	printf("%s detach Failed\n",t_name);
	 }

	sleep(4);
	if(0 != pthread_cancel(thread1))
	{
		printf("Pthread Cancel Failed %s",strerror(errno));

	}
	if(0 != pthread_cancel(thread2))
	{
		printf("Pthread Cancel Failed %s",strerror(errno));

	}

	if( -1 == execl("process_reboot","reboot",NULL))
	{
		printf("Excel Failed %s\n",strerror(errno));
		exit(0);
	}
	pthread_exit(NULL);





return NULL;
}
void * thread_routine(void * arg)
{
	char *t_name = (char *)arg;

	 if(0 != pthread_detach(pthread_self()))
	 {
	 	printf("%s detach Failed\n",t_name);
	 }

	for(int i=0;i<10;i++)
	{
		printf("%s Active %d\n",t_name,i);
		sleep(1);
	}

	pthread_exit(NULL);

return NULL;
}
