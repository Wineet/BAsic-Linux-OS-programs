/*before running check weather IPC key Is available
 * run code in previlege MOde*/

#include<stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/sem.h>



#define IPCKEY 1000      // Check key before assign weather it is avialiable or not
#define NoSEMA 1

struct shareMem{		// Structure for shared Memory

	int var1;
	int var2;
	char buf[1024];

};

/* Union Is needed for Semaphore Control Operations*/
union semun {
               int              val;    /* Value for SETVAL */
               struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
               unsigned short  *array;  /* Array for GETALL, SETALL */
               struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                           (Linux-specific) */
           };


union semun u1;
struct sembuf semobj;       // Semaphore Object Which will be handling semaphores

void main(int argc, char* argv[])
{



	struct shareMem *ptr=NULL;		// Pointer to shared Memoery
	
/*---------- Semaphore Init--------------*/

  int id2=semget(IPCKEY,NoSEMA,IPC_CREAT|0600);	// Creating Semanphores  
						// P1= IPC Key
						// P2= Number of semaphores to be 
						// created
						// P3= Flags
						// If sucessful returns 
						// semaphore identifier	

	if(id2==-1)
	{
		printf("Error in SEMGET");
		exit(1);
	
	}

	u1.val=1;			// Initial Value of semaphore
int ret_sem=semctl(id2,0,SETVAL,u1);	// semaphore control operation
					// P1=ID of semaphore
					// P2='n'th semaphore operations are done
					// according to flag set of P3
					// P3= Flag to do control Operation
					//P4= u1>> setting initial value u1.val
					//on sucess and for SETVAL flag returns 0
	
	if(ret_sem!=0)
	{
		printf("SemCtl Unsucessful\n");
	}
	ret_sem=semctl(id2,0,GETVAL);
	printf("Semaphore inital value=%d\n",ret_sem);

/*----------------------------*/	
/*Shared Memory Section*/

int id=shmget(IPCKEY,4*4096,IPC_CREAT|0666);	// Assign shared Memory segments
						//P1= IPCKEY
					       //P2=sizeof shared Mem want to Assign
						//P3= Flag for shared memory
						//and prevelige setting	
						// returns ID of shared segment to 
						// get pointer to shared memory use
						// >>shmat<< 
						// for control >>shmctl()<<
if(id==-1)
{
	printf("ERROR In SHMGET\n");
}

ptr=shmat(id,0,0);		// Shared Memory Pointer return
				//P1= ID for Shared Segment
				//P2= to give address for shared mem
				//if null,system will provide its own
				//address
				//P3 = Flags choose as per required
if(ptr==NULL)
{
	printf("Error in SHMCTL\n");
}





int ret= fork();	// for parent ret value is child PID
			// for child it is zero	
// creating child which will access  the shared Memory
// both have same address space so both will be sharing same key
// child and parent both will be accessable to shared memory

printf("ret val=%d\n",ret);	// Parent will print Child PID
				// Child Will Print Zero

	if(ret>0)	// Parent will enter in this block only  and do Work
	{	

		long int i=60000;
		while(--i)
		{	
			// Applying Semphores to avoide Race Condition
			// Locking
			semobj.sem_num = 0;        /* Operate on semaphore 0 */
	                semobj.sem_op = -1;         /* Decrement value by one */
                        semobj.sem_flg = 0;
			semop(id2,&semobj,1);   // performs operation on
		       				//selected sema phore object
						// to do semaphore operations
						// P1= SemID
						// P2=SemaPhore Object Address
						// P3=Number of operations
			ptr->var1++;
			ptr->var2++;
			//Unlocking after operations
                       semobj.sem_num = 0;        /* Operate on semaphore 0 */
                        semobj.sem_op = 1;         /* Increment value by one */
                        semobj.sem_flg = 0;
                        semop(id2,&semobj,1);

		}
	}
	

	if(ret==0)	// Child will enter in this block only
	{
		long int i=60000;
		while(--i)
		{
			// Applying Semphores to avoide Race Condition
                        // Locking
                        semobj.sem_num = 0;        /* Operate on semaphore 0 */
                        semobj.sem_op = -1;         /* Decrement value by one */
                        semobj.sem_flg = 0;
                        semop(id2,&semobj,1);   // performs operation on
                                                //selected sema phore object
                                                // to do semaphore operations
                                                // P1= SemID
                                                // P2=SemaPhore Object Address
                                                // P3=Number of operations
                        ptr->var1--;
                        ptr->var2--;

                        //Unlocking after operations
                        semobj.sem_num = 0;        /* Operate on semaphore 0 */
                        semobj.sem_op = 1;         /* Increment value by one */
                        semobj.sem_flg = 0;
                        semop(id2,&semobj,1);

		}
		exit(0);	// Child DEAD
	
	
	}
	
   	
     /*  IPC Lock MEchanism  Semaphore is implemented  <<
      *  Race conditon won;t occure so increment decrement  happen same time
      *  and output zero
      *  if no semaphore implemented the output values are different and
      *  unpredictable
      * */
	
     /* Parent Should Clean Up child process and free its resources after child get terminated*/
   	  while(1)
	{
		int status=0,ret2=0;
		ret2=waitpid(-1,&status,0);	// return value of terminated Child
						// parent will blocked here and
						// waiting for child termination
		// After child termination parent will unblock and
		//  perform its operation and exit status of child
		//   is stored in status 
		//  p1= for  specific Child trmination only (-1 for any child)
		//  p2= exit status it will store
		//  p3= other  options

	 if(ret2>0){ 
      printf("pid of the child process cleaned-up is %d\n", ret2);
    if(WIFEXITED(status))     //normal termination of the process
    {
       if(WEXITSTATUS(status) == 0){ //normal and successful

         printf(" Normal and Succ.  is %d\n", WEXITSTATUS(status));
       }
       else
       {		//normal, but not successfull
	       printf("Normal but Unsucee  process is %d\n", WEXITSTATUS(status));

       }
    }

 
    else
    { 	//abnormal (did not succeed)
         ret = WTERMSIG(status);
         printf("the signal for abnormal termination is %d\n", ret);
    }
	
	 }
	 else
	 {
  printf("Shared Mem final Value VAlue var1=%d,var2=%d\n",ptr->var1,ptr->var2);
 
	 	break;		// After Cleaning Up All Child

	 }
	}


}
