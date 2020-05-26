/*
 * Author:   vinit 
 * Task:     --Here--
 * Hw  : 	   R-pi 3B+
 * Platform: Linux
 *
 */


#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<pthread.h>

/*Function Declaration Here*/
long int global_var1=5;
long int global_var2=10;
long int global_var3=15;

/*Global Variable Here*/
pthread_mutex_t mutex_lock;

void *inc_thread(void *arg_ptr)
{
	
 for(long int i=0;i<60000;i++ )
 {
	 pthread_mutex_lock(&mutex_lock);
	 global_var1++;
	 global_var2++;
	 global_var3++;
	 pthread_mutex_unlock(&mutex_lock);

 }

return NULL;
}

void *dec_thread(void *arg_ptr)
{
	
 for(long int i=0;i<60000;i++ )
 {	
	 pthread_mutex_lock(&mutex_lock);
	 global_var1--;
	 global_var2--;
	 global_var3--;
	pthread_mutex_unlock(&mutex_lock);
 }

 return NULL;
}


int  main(int argc,char *argv[])
{

	int ret_val=0;
	pthread_t thread1;
	pthread_t thread2;

	if(0 != pthread_mutex_init(&mutex_lock,NULL))
	{
		printf("Mutex_init Failed\n");
		
	}

	if(0 != pthread_create(&thread1,NULL,inc_thread,NULL))
	{
		printf("pthread Error %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	if(0 != pthread_create(&thread2,NULL,dec_thread,NULL))
	{
		printf("pthread Error %s\n",strerror(errno));
		exit(EXIT_FAILURE);
	}
	pthread_join(thread1,NULL);
	pthread_join(thread2,NULL);
	
         printf("G1 %ld\n",global_var1);
	 printf("G2 %ld\n",global_var2);
	 printf("G3 %ld\n",global_var3);



return 0;
}

