/* Basic thread Program
 * while compiling link thread Liberary 
 *gcc -o t thread1.c -lpthread
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

struct dat{

	int var1;
	int var2;

};



void main()
{
	int ret;
	pthread_create(&thread1,NULL,&f1,"Vinit");	//Creates a new Thread
					         	//P1=thread
							//P2= attribute for creating
							//thread For Null 
							//take default
							//P3=function Address
							//p4=Argument which will 
							//pass through function

	pthread_create(&thread2,NULL,&f2,"Thread1");	//Creates a new Thread
	
	pthread_join(thread1,NULL);		// function wait upto terminate
       						//perticular thread
						//P1=thread 
						//P2=to copy exit status in the
						//location pointed by pointer
	pthread_join(thread2,NULL);


}

/* From thread we Can return void pointer to calling thread*/

static void *f1(void *arg){

	if((char *)arg!=NULL)
 printf("Arg=%s\n",(char *)arg);
	
   while(1)
 {
	/*Put A loop which you want to execute*/

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
   while(1)
 {
	/*Put A loop which you want to execute*/

 }
pthread_exit(NULL);		// For Exit of corresponding  thread
				// other threads continue their work
				// P1= Stores the exit status pointed by
				// pointer it must not be in exiting thread stack
/* Don't Use Exit Here it will Exit complete process*/

}



