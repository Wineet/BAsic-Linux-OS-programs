#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
void * IncThread(void *);
void * DecThread(void *);

long GlbVar1,GlbVar2,GlbVar3;
int main()
{
	pthread_t Thread1_Id,Thread2_Id,Thread3_Id,Thread4_Id;

	pthread_create(&Thread1_Id,NULL,&IncThread,NULL);
	pthread_create(&Thread2_Id,NULL,&IncThread,NULL);
	pthread_create(&Thread3_Id,NULL,&DecThread,NULL);
	pthread_create(&Thread4_Id,NULL,&DecThread,NULL);
	printf("GlbVar1 = %lu\n GlbVar2 = %lu\n GlbVar3 = %lu \n", GlbVar1,GlbVar2,GlbVar3);

	pthread_join(Thread1_Id,NULL);
	pthread_join(Thread2_Id,NULL);
	pthread_join(Thread3_Id,NULL);
	pthread_join(Thread4_Id,NULL);
}
void * IncThread(void *arg)
{
	long i;
	for(i=0;i<65535;i++)
	{
		GlbVar1++;
		GlbVar2++;
		GlbVar3++;
	}
	pthread_exit(NULL);
	return (void*)NULL;
}
	
void * DecThread(void *arg)
{
	long i;
	for(i=0;i<65535;i++)
	{
		GlbVar1--;
		GlbVar2--;
		GlbVar3--;
	}
	pthread_exit(NULL);
	return (void*)NULL;


}
