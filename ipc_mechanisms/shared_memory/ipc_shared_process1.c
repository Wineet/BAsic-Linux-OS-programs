#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define PAGE_SIZE 4096
#define IPC_KEY  1244     // Dummy Keys

//Structure for shared memory

typedef struct {

	int value ;
	char arr[100];

}shared_struct;


int main()
{
	int ret_val = 0;
	int shm_id  = 0;
	void *shared_mem_ptr = NULL;
 	shared_struct * shared_ptr=NULL;
	
	printf("Shared Memory IPC...\n");



 	shm_id = shmget((key_t)IPC_KEY,1*PAGE_SIZE, 0777|IPC_CREAT);
	
	if(shm_id == -1)
	{
	  printf("Error in shm_id ErrorCode=%d\n",shm_id);
	  exit(EXIT_FAILURE);
	}
	
	shared_mem_ptr =  shmat(shm_id,(void *)0,0);	// Read write both acess

	if( (void *)-1 == shared_mem_ptr)
	{
	  printf("Error in shmat = %d\n ",shared_mem_ptr);
	  exit(EXIT_FAILURE);
	}

	printf("Writing to shared Memory..\n");
	shared_ptr = (shared_struct *) shared_mem_ptr;
	printf("shared_ptr =%p\n",shared_ptr);
	shared_ptr->value = 10 ;
	strncpy(shared_ptr->arr,"Hi Are you there.." ,100);

	printf("detaching Memory....\n");
	
	if(0 != shmdt(shared_mem_ptr))
	{
		printf("Error While Detaching Memory\n");
		exit(EXIT_FAILURE);
		
	}
return 0;

}
