/*
 *Author: Vinit
 *OS: Linux
 *Platform: R-pi
 *Task: Multi-threading and Race Condition
 *
 * */


#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<stddef.h>
#include<string.h>

#include<pthread.h>
#include<unistd.h>
/*Global Variable taking as resource*/
long global_var1=0;
long global_var2=0;
long global_var3=0;

void *thread_increment(void *arg);
void *thread_decrement(void *arg);



void main()
{

	int ret_val=0;
	pthread_t thread_id[4]={0};
	
	typedef void* (*p)(void *);			// For function pointer registry
	void *thread_ret =NULL;

	p ptr[2]={0};
	ptr[0]=thread_increment;
	ptr[1]=thread_decrement;
	
	printf(" Process PID=%d \n",getpid());

	for(int i=0,j=0;i<4;i++)
	{
	
		ret_val = pthread_create(thread_id+i,NULL, (i>1?(*(ptr+1)):(*ptr)) ,NULL);		//using Default Arguments and default attributes
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


	return;
}



void *thread_increment(void *arg)
{
	while(1);		  // toobserve mem_info
	for(int i=0;i<65500;i++)
	{
		global_var1++;
		global_var2++;
		global_var3++;
	
	}
	printf("Thread O/P inc\n var1=%d\n var2=%d\n var3=%d\n\n",global_var1,global_var2,global_var3);

	pthread_exit(NULL);

return(void *)NULL;
}



void *thread_decrement(void *arg)
{
	while(1);			//stuck thread here to see stack in proc_info
	for(int i=0;i<65500;i++)
	{
		global_var1--;
		global_var2--;
		global_var3--;
	
	}

	printf("Thread O/P Dec\n var1=%d\n var2=%d\n var3=%d\n\n",global_var1,global_var2,global_var3);

	pthread_exit(NULL);

return(void *)NULL;
}
