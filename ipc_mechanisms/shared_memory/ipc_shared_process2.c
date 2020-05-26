#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <stdlib.h>
#define PAGE_SIZE 4096
#define IPC_KEY  1244     // Dummy Keys

typedef struct {

	int value;
	int arr[100];

}shared_struct;


void main()
{
	int ret_val = 0;
	int shm_id  = 0;
	void *shared_mem_ptr = NULL;
 
	
	printf("Shared Memory IPC...\n");

 	shm_id = shmget(IPC_KEY,1*PAGE_SIZE,0777|IPC_CREAT);
	
	if(shm_id == -1)
	{
	  printf("Error in shm_id ErrorCode=%d\n",shm_id);
	
	}
	
	shared_mem_ptr =  shmat(shm_id,NULL,0);	// Read write both acess

	if( (void *)-1 == shared_mem_ptr)
	{
	  printf("Error in shmat = %d \n",shared_mem_ptr);
	
	}

	printf("Reading from shared Memory\n");
    shared_struct *shared_ptr =  (shared_struct *) shared_mem_ptr;

    printf("Shared Value =%d\n",shared_ptr->value);
    printf("shared arr  =%s\n",shared_ptr->arr);

    if(0 != shmdt(shared_mem_ptr))
    {
    	printf("Error in detaching...\n");
	exit(EXIT_FAILURE);
    }

    printf("Deleting SHared Memory\n");

    if(0  != shmctl(shm_id,IPC_RMID,0))
    {
    	printf("shmctl Error..\n");
	exit(EXIT_FAILURE);
    }

}
