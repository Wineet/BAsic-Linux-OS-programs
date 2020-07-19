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
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>

int main()
{
  int ret_val;
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