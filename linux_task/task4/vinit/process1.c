/*
 *Author: Vinit
 *OS:     Linux
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
#include<sys/ipc.h>
#include<sys/shm.h>

#define PAGE_SIZE 4096
#define SHM_IPC_KEY 1111	// Assigning Random Key

/*Global Variable taking as resource*/
typedef struct shared_mem {

long global_var1; 
long global_var2;
long global_var3;

}sm_s;



int main(int argc, char * argv[])
{
	int shm_id=0;

	sm_s *sm_ptr=NULL;
	printf("Process 1 Started \n");
	
	/* Creating Shared Memory and obtaining IPC key Here
	 *
	 * */
	shm_id=shmget(SHM_IPC_KEY,PAGE_SIZE*1,IPC_CREAT|0777);	// With all permission
	if(-1==shm_id)
	{
		printf("shmget error %s",strerror(errno));
		return EXIT_FAILURE;
	}

	/*
	 * shared memory attachment with shmid
	 * attach with segment no specific address
	 * both read and write access is given with flag=0
	 *
	 * on success return shared memory attached address
	 * */
	void * const shared_at = shmat(shm_id,NULL,0);		
	if(-1 ==(int) shared_at)
	{
		printf("shmat Failed %s\n",strerror(errno));
		return EXIT_FAILURE;
	}
	/*memory is attached and we recieved it's pointer*/
	
	sm_ptr= (sm_s *)shared_at;

	for (int i=0;i<65530;i++)
	{
		sm_ptr->global_var1++;	
		sm_ptr->global_var2++;	
		sm_ptr->global_var3++;	
	
	}

	for (int i=0;i<65530;i++)
	{
		sm_ptr->global_var1--;	
		sm_ptr->global_var2--;	
		sm_ptr->global_var3--;	
	}

	printf("P1>> global var 1 %ld\n",sm_ptr->global_var1);
	printf("P1>> global var 2 %ld\n",sm_ptr->global_var2);
	printf("P1>> global var 3 %ld\n",sm_ptr->global_var3);
	
	/*
	 *  mark shared memory segment for Destroy
	 * segment destroyed when laast process will get detached
	 *
	 * */
	if(shmctl(shm_id,IPC_RMID,0))		
	{
		printf("shmctl error");
		return EXIT_FAILURE;
	}

	printf("Process1 detaching Memory\n");
	if(0 != shmdt(shared_at))
	{
		printf("Detaching memory Failed\n");
		return EXIT_FAILURE;
		
	}



}
