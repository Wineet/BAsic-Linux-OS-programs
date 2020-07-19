#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<errno.h>
#include<limits.h>


#define StackSize 16384U	//<<Vinit: Try to use PAGE SIZE as a standard then do use by multipling>>
#define PTHREAD_STACK_MIN  (4096*4)
void * IncThread(void *);
void * DecThread(void *);
pthread_mutex_t  Lock;
long GlbVar1 = 10,GlbVar2 = 20,GlbVar3 = 30;
int main()
{
	pthread_t Thread1_Id,Thread2_Id,Thread3_Id,Thread4_Id;

   pthread_attr_t  ThreadAttr;
   int retval = 0U;
   
   pthread_mutex_init(&Lock,NULL);
   
   retval = pthread_attr_setstacksize(&ThreadAttr,StackSize);
   if(retval != 0)
   {
      printf("set stack size error :::: error no. -->  %d \n",retval);
      if(EINVAL == retval) puts("match");
      if(PTHREAD_STACK_MIN > StackSize) puts("less size");
      exit(0);
   }
   pthread_attr_init(&ThreadAttr);
	retval = pthread_create(&Thread1_Id,&ThreadAttr,&IncThread,NULL);
   if(retval != 0)
   {
      printf("Thread1 :::: error no. -->  %d\n",retval);
      exit(0);
   }
	retval = pthread_create(&Thread2_Id,&ThreadAttr,&IncThread,NULL);
   if(retval != 0)
   {
      printf("thread 2 :::: error no. -->  %d\n",retval);
      exit(0);
   }
	retval = pthread_create(&Thread3_Id,&ThreadAttr,&DecThread,NULL);
	if(retval != 0)
   {
      printf("thread 3:::: error no. -->  %d\n",retval);
      exit(0);
   }
   retval = pthread_create(&Thread4_Id,&ThreadAttr,&DecThread,NULL);
	if(retval != 0)
   {
      printf("thread 4 :::: error no. -->  %d\n",retval);
      exit(0);
   }
   printf("GlbVar1 = %lu\n GlbVar2 = %lu\n GlbVar3 = %lu \n", GlbVar1,GlbVar2,GlbVar3);

	pthread_join(Thread1_Id,NULL);
	pthread_join(Thread2_Id,NULL);
	pthread_join(Thread3_Id,NULL);
	pthread_join(Thread4_Id,NULL);
   
   puts("before mutex destroy");

   pthread_mutex_destroy(&Lock); 
      puts("destroyd");
      return 0;
}
void * IncThread(void *arg)
{
	long i;
   pthread_mutex_lock(&Lock);
         puts("LOCK INCTHREAD");
	for(i=0;i<65535;i++)
	{
		GlbVar1++;
		GlbVar2++;
		GlbVar3++;
	}
   pthread_mutex_unlock(&Lock);
	pthread_exit(NULL);
   
   puts("unLOCK INCTHREAD");
	return (void*)NULL;
}
	
void * DecThread(void *arg)
{
	long i;
      pthread_mutex_lock(&Lock);
      puts("LOCK DECTHREAD");
	for(i=0;i<65535;i++)
	{
		GlbVar1--;
		GlbVar2--;
		GlbVar3--;
	}
   
   pthread_mutex_unlock(&Lock);
	pthread_exit(NULL);
      
	return (void*)NULL;


}
