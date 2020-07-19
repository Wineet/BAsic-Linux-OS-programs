/*
 *Author: Vinit
 *OS: Linux
 *Platform: R-pi
 *Task: Multi-threading and resolve Race Condition by putting POSIX Mutex Lock
 *
 * */


#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<stddef.h>
#include<string.h>

#include<pthread.h>
#include<unistd.h>

#define PAGE_SIZE 4096
/*Global Variable taking as resource*/
long global_var1=1; 
long global_var2=2;
long global_var3=3;

void *thread_increment(void *arg);
void *thread_decrement(void *arg);
	
pthread_mutex_t mutex_l;

void main()
{

	int ret_val=0;
	pthread_t thread_id[4]={0};
	
	typedef void* (*p)(void *);			// For function pointer registry
	void *thread_ret =NULL;
	
	ret_val = pthread_mutex_init(&mutex_l,NULL);		// Default Mutex Attributes
	if(0 != ret_val)
	{
		printf("Mutex_init Failed %s\n",strerror(ret_val));
		exit(EXIT_FAILURE);
	}

	pthread_attr_t thread_attr;
	
	p ptr[2]={0};
	ptr[0]=thread_increment;
	ptr[1]=thread_decrement;
	
	
	for(int i=0,j=0;i<4;i++)
	{
		 ret_val=pthread_attr_init(&thread_attr);						// Thread attribute init 
		
		if(0 != ret_val)
		{
			printf("attr_init Failed %s\n",strerror(ret_val));
			exit(EXIT_FAILURE);
		}
		ret_val= pthread_attr_setstacksize(&thread_attr,PAGE_SIZE*4);			// Initilising thread stack size Minimum is 16Kand should be multiple of page
		if(0 != ret_val)
		{	
			printf("set stack size failed %s\n",strerror(ret_val));
			exit(EXIT_FAILURE);
		}

		ret_val = pthread_create(thread_id+i,&thread_attr, (i>1?(*(ptr+1)):(*ptr)) ,(void *)NULL);		//using Default Arguments and changed attributes
		if(0 != ret_val)
		{
			printf("pthread create Failed.. \n");
			exit(EXIT_FAILURE);
		}
	}


	for(int i=0;i<4;i++)
	{
		ret_val=pthread_join(thread_id[i],&thread_ret);
		if(0 != ret_val)
		{
			printf("Pthread Join Error\n");
		}
		else
		{
			printf("Thread %d exit with ret=%s\n",thread_id[i],(char *)thread_ret);
		
		}

	}
	
		pthread_mutex_destroy(&mutex_l);			// Clear Mutex 

	printf("Final O/P Val\n var1=%d\n var2=%d\n var3=%d\n\n",global_var1,global_var2,global_var3);
	return;
}

/*Insted of covering complete loop code with mutex
 * It is better to cover Read write modify action
 * with Mutex Lock*/

void *thread_increment(void *arg)
{


	for(int i=0;i<65500;i++)
	{

		pthread_mutex_lock(&mutex_l);
	
		global_var1++;
		global_var2++;
		global_var3++;
	
		pthread_mutex_unlock(&mutex_l);
	}


//	printf("Thread O/P inc\n var1=%d\n var2=%d\n var3=%d\n\n",global_var1,global_var2,global_var3);

	pthread_exit(NULL);

return(void *)NULL;
}

/*
 *We can do Exception Handline for mutex lock & unlock Ret_val
 *It will make code more rigid
 * */

void *thread_decrement(void *arg)
{
	

	for(int i=0;i<65500;i++)
	{

		pthread_mutex_lock(&mutex_l);
		
		global_var1--;
		global_var2--;
		global_var3--;
	
		pthread_mutex_unlock(&mutex_l);
	}



//	printf("Thread O/P Dec\n var1=%d\n var2=%d\n var3=%d\n\n",global_var1,global_var2,global_var3);

	pthread_exit(NULL);

return(void *)NULL;
}
