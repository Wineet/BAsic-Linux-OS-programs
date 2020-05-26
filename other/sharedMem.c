/*before running check weather IPC key Is available
 * run code in previlege MOde*/

#include<stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>


#define IPCKEY 1000      // Check key before assign weather it is avialiable or not

struct shareMem{		// Structure for shared Memory

	int var1;
	int var2;
	char buf[1024];

};


void main(int argc, char* argv[])
{

	struct shareMem *ptr=NULL;			// Pointer to shared Memoery
	
int id=shmget(IPCKEY,4*4096,IPC_CREAT|0666);	// Assign shared Memory segments
						//P1= IPCKEY
						//P2= size of shared Mem want to Assign
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

		long int i=10000;
		while(--i)
		{
			ptr->var1++;
			ptr->var2++;
		}
	}
	

	if(ret==0)	// Child will enter in this block only
	{
		long int i=10000;
		while(--i)
		{
			ptr->var1--;
			ptr->var2--;
		}
		exit(0);	// Child DEAD
	
	
	}
	
     printf("Shared Mem Final Value VAlue var1=%d,var2=%d\n",ptr->var1,ptr->var2);
	
     /* Due to parallel processing of Parent and Child it won;t come zero
      * it give you some different value ideally it should give you zero
      * to achieve that we need to use 
      * >> IPC Lock MEchanism like Semaphore or Mutex <<
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
	 	break;		// After Cleaning Up All Child

	 }
	}


}
