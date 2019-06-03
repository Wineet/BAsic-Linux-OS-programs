/* Basic thread Program
 *Achieveing synchronisation using
 *Scheduling Policy 
 *gcc -o t thread1.c -lpthread
 *while Launching pin process to a processor
 * use taskset 0x00000001 ./<<executable name>>
 * to achieve synchronisation
*/

#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>

static void *f1(void *);
static void *f2(void *);

pthread_t thread1,thread2;

pthread_attr_t attr1,attr2;	//Attributes for threads


struct dat{

	int var1;
	int var2;

};

struct dat data;
struct sched_param p1,p2,p3;
void main()
{
	int ret;

	p3.sched_priority = 44;
	pthread_setschedparam(pthread_self(),SCHED_FIFO,&p3);


	pthread_attr_init(&attr1);
	pthread_attr_init(&attr2);

	p1.sched_priority=45;	
	p2.sched_priority=40;

	pthread_attr_setschedpolicy(&attr1,SCHED_FIFO);
	pthread_attr_setschedpolicy(&attr2,SCHED_FIFO);

	pthread_attr_setschedparam(&attr1,&p1 );
	pthread_attr_setschedparam(&attr2,&p2 );


	pthread_create(&thread1,&attr1,&f1,"thread1");	//Creates a new Thread
					         	//P1=thread
							//P2= attribute for creating
							//thread For Null 
							//take default
							//P3=function Address
							//p4=Argument which will 
							//pass through function

	pthread_create(&thread2,&attr2,&f2,"Thread2");	//Creates a new Thread
	
	pthread_join(thread1,NULL);		// function wait upto terminate
       						//perticular thread
						//P1=thread 
						//P2=to copy exit status in the
						//location pointed by pointer
	pthread_join(thread2,NULL);
	
	printf("Final Value var1=%d,var2=%d\n",data.var1,data.var2);

}

/* From thread we Can return void pointer to calling thread*/


/* Due to race condition values won;t be same as expected because of parallel 
 * processing and race cndition so we need to to put 
 * Some locks like Mutex<<
 */


static void *f1(void *arg){

	if((char *)arg!=NULL)
 printf("Arg=%s\n",(char *)arg);

	int i=60000;
   while(i--)
 {
	/*Put A loop which you want to execute*/

	data.var1++;
	data.var2++;
 
 }

pthread_exit(NULL);		// For Exit of corresponding  thread
				// other threads continue their work
				// P1= Stores the exit status pointed by
				// pointer it must not be in exiting thread stack
	
/* Don't Use Exit Here it will Exit complete process*/
}




static void *f2(void *arg){

	if((char *)arg!=NULL)
	 printf("Arg=%s\n",(char*)arg);

	int i=60000;
     	while(i--)
 	{
		data.var1--;
		data.var2--;

	/*Put A loop which you want to execute*/
	
	
 	}

pthread_exit(NULL);		// For Exit of corresponding  thread
				// other threads continue their work
				// P1= Stores the exit status pointed by
				// pointer it must not be in exiting thread stack
/* Don't Use Exit Here it will Exit complete process*/

}



