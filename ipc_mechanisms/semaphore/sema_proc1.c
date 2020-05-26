#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SEM_IPC_KEY 3333
#define SHARE_IPC_KEY 1220

#define PAGE_SIZE 4096

// Shared Memory Code
typedef struct {

	int val1;
	int val2;
	int val3;


}shared_struct;

int shm_id = 0;
void *shared_ptr = NULL;


void creat_attach_shared_mem(void)
{   
    shm_id = shmget(SHARE_IPC_KEY,1*PAGE_SIZE,0777|IPC_CREAT);
	
    if(shm_id == -1)
    {
    	printf("Error shmget\n");
	exit(EXIT_FAILURE);
    }
   
    shared_ptr = shmat(shm_id,NULL,0);

    if((void *)-1 == shared_ptr)
    {
    	printf("Error shmat\n");
	exit(EXIT_FAILURE);
    }

    printf("Shared Memory Created...\n");

return ;

}

void deattach_delete_memory(void)
{
	if(0 != shmdt(shared_ptr))
	{
		printf("Error Detaching\n");
		exit(EXIT_FAILURE);
	}
	

	if(0 != shmctl(shm_id,IPC_RMID,0))
	{
	
		printf("Error shmctl\n");
		exit(EXIT_FAILURE);
	}
	printf("Shared memory deattached and Deleted\n");

return;

}

// Shared Memory Code END Here
//



struct sembuf sem_buf;
union semun
{
	int val;


}sem_union;





void main()
{
	printf("Semaphore Program....\n");

	int sem_set_identifier=0;


	shared_struct *shared_mem_ptr = NULL;
	creat_attach_shared_mem();
	shared_mem_ptr = (shared_struct *) shared_ptr;



	 sem_set_identifier = semget( (key_t)SEM_IPC_KEY ,1, 0666 | IPC_CREAT );	//Semaphore creation


 	 if (-1 == sem_set_identifier)
	 {
	
		printf("Semget Failed...\n");
		exit(EXIT_FAILURE);
	
         }

	 sem_union.val=1;	// Initial Value setting

	if(-1 == semctl(sem_set_identifier,0,SETVAL,sem_union))	
	{
		printf("Error In semctl...\n");
		exit(EXIT_FAILURE);
	}

	printf("Getval init%d\n",semctl(sem_set_identifier,0,GETVAL));

	 sem_buf.sem_num  = 0;
	 sem_buf.sem_op   = -1;
	 sem_buf.sem_flg = SEM_UNDO;
	semop(sem_set_identifier,&sem_buf,1);

	printf("getval Before = %d\n",semctl(sem_set_identifier,0,GETVAL));

	for (unsigned int i=0;i<65300;i++)
	{
		shared_mem_ptr->val1++;
		shared_mem_ptr->val2++;
		shared_mem_ptr->val3++;
	}

	sem_buf.sem_num = 0;
	sem_buf.sem_op  = +1;
	sem_buf.sem_flg = SEM_UNDO;
	semop(sem_set_identifier,&sem_buf,1);
	
	printf("getval After=%d\n", semctl(sem_set_identifier,0,GETVAL));

	printf("proc1 shared_mem_ptr->val1=%d\n",shared_mem_ptr->val1);
	printf("proc1 shared_mem_ptr->val2=%d\n",shared_mem_ptr->val2);
	printf("proc1 shared_mem_ptr->val3=%d\n",shared_mem_ptr->val3);


//	deattach_delete_memory();

	return;
}
