/* implementation steps
#include <semaphore.h>



  1. int sem_init(sem_t *sem, int pshared, unsigned int value);
   *sem_init() returns 0 on success; on error, -1 is returned, and errno
      is set to indicate the error.
   ERRORS         top
      EINVAL value exceeds SEM_VALUE_MAX.

      ENOSYS pshared is nonzero, but the system does not support process-
              shared semaphores
  2. int sem_destroy(sem_t *sem);
+
      Link with -pthread.

   sem_destroy() destroys the unnamed semaphore at the address pointed
   to by sem.

   Only a semaphore that has been initialized by sem_init(3) should be
   destroyed using sem_destroy().

   Destroying a semaphore that other processes or threads are currently
   blocked on (in sem_wait(3)) produces undefined behavior.

   Using a semaphore that has been destroyed produces undefined results,
   until the semaphore has been reinitialized using sem_init(3).
   sem_destroy() returns 0 on success; on error, -1 is returned, and
       errno is set to indicate the error.
ERRORS         top
       EINVAL sem is not a valid semaphore.
       
       
   Link with -pthread.*/
   
   
#include<stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ipc.h>   // for ipc
#include <sys/shm.h>   //for shared  mem
#include<sys/wait.h>
 #include <pthread.h> // for creating thread
#include <string.h>
#include <fcntl.h>  // for O_ * flages
#include <sys/stat.h>		 /* For mode constants */



#include<unistd.h> //for sleep

#include<semaphore.h>  // for semaphore
#define KEY2 1432
#define message(x) //printf("\n%s\n",x)
#define chkret(x,y) //printf("\n%s : %d : %s \n",x,y,strerror(y))
#define PAGESIZE 16384U


int main()
{
  int ret_val,SHM_Id2,semint;

  sem_t *semLock;

  
  SHM_Id2 = shmget(KEY2,PAGESIZE,0666|IPC_CREAT); 
  
	 if(-1 == SHM_Id2)
	 {
		perror("2shmget failed");
	 }
	 
	 message("2shmget success");
	  semLock = shmat(SHM_Id2,NULL,0666);
	   //printf("\np2:-Var1 = %d\np2:-Var2 = %d\np2:-Var3 = %d\n",structVar->Var1,structVar->Var2,structVar->Var3);
	 
	   
		if ((void*)-1 == semLock)
		{
		   perror("2shmat failed");
		}
		message("2shmat success");
  
  
	 
	 message("triggering semopen2");
	 semint = sem_init(semLock,1,1);
	 if(-1==semint)
	   {
		 
		   perror("sem_init failed");
	   }
	 chkret("sem init returns", semint);
	 
	 
	 
  
  //   semLock = sem_open(SEMAPHORE,O_CREAT/*,S_IRUSR | S_IWUSR,1*/);
	  
  if(0==fork())
  {
     printf("process1\n");
     ret_val=   execl("./Program1.o","Program1.o",NULL);
     if(-1 == ret_val)
				{
					printf("execl Failed \n");
				
					exit(EXIT_FAILURE);
					
				}
     
  }
  else{
  printf("process2\n");
  ret_val=execl("./Program2.o","Program2.o",NULL);
  if(-1 == ret_val)
				{
					printf("execl Failed \n");
				
					exit(EXIT_FAILURE);
					
				}
  }
  
  
   return 0;
}
