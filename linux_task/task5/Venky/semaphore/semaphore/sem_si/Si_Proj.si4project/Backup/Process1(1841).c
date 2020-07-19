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


#define KEY 2942U
#define PAGESIZE 16384U

#define message(x) printf("\n%s\n",x)
#define chkret(x,y) printf("\n%s : %d : %s \n",x,y,strerror(y))
#define SEMAPHORE (const char *)"Sync1Sem"
pthread_mutex_t  Lock;
struct var
{
	//int P;
   int Var1;
   int Var2;
   int Var3;
}SHM_Struct;

/*
	sem_wait():
	


*/

void* incVar(struct var * variable);
void* decVar(struct var * variable);


void print()
{
   int i;
   for(i=0;i<10;i++)
   {
   printf("process : p1 \n");
   sleep(1);
   }
}

struct var *structVar;

int main()
{
	message("in main");
   int SHM_Id = 0U;
   pthread_t Thread1,Thread2;
   int handleJoin1,handleJoin2,semint;
   int semwait,sempost;
   /*
         creat shared memory
   */
   
   message("triggering shmget");
   SHM_Id = shmget(KEY,PAGESIZE,0666|IPC_CREAT); // read write execute permission 
   
   if(-1 == SHM_Id)
   {
      perror("shmget failed");
   }
   
   message("shmget success");
   //   void *shmat(int shmid, const void *shmaddr, int shmflg);
   /*
         attach shared memory
   */
   
   message("triggering shmat");
   structVar = shmat(SHM_Id,NULL,0666);
  //printf("\np1:-Var1 = %d\np1:-Var2 = %d\np1:-Var3 = %d\n",structVar->Var1,structVar->Var2,structVar->Var3);

   /*
       sem_init call
       int sem_init(sem_t *sem, int pshared, unsigned int value);

       pshared -- value 0 (if semaphore is shared between threads)
       			  value non 0 (if semaphore is shared between processes)
   */
	//sem_init(&(structVar->semLock),1,0);
  

   if ((void*)-1 == structVar)
   {
      perror("shmat failed");
   }
   message("shmat success");

/*********************************************************************************************************************************/
   //creating and attaching semLock
sem_t *semLock;
   SHM_Id2 = shmget(KEY2,PAGESIZE,0666|IPC_CREAT); 
	 
		if(-1 == SHM_Id2)
		{
		   perror("2shmget failed");
		}
		
		message("2shmget success");
		 semLock = shmat(SHM_Id2,NULL,0666);
		  //printf("\np1:-Var1 = %d\np1:-Var2 = %d\np1:-Var3 = %d\n",structVar->Var1,structVar->Var2,structVar->Var3);
		
		  
		   if ((void*)-1 == semLock)
		   {
			  perror("2shmat failed");
		   }
		   message("2shmat success");
/***********************************************************************************************************************************/
   
   //message("sem_init success");
   /*
         create thread
   */

    semwait = sem_wait(semLock);
   chkret("semwait returns",semwait );
   structVar->Var1 = 1;
   structVar->Var2 = 4;
   structVar->Var3 = 3;

   /****************************** creat threads*************************************/
   message("p1--->creating thread 1");
   if (0 != pthread_create(&Thread1,NULL,(void*)incVar,(void*)structVar))
   {
      printf("P1:Thread1 create error : %s\n",strerror(errno));
      return EXIT_FAILURE;
   }
   message("p1--->thread 1 success");
   message("p1--->creating thread 2");
   if (0 != pthread_create(&Thread2,NULL,(void*)decVar,(void*)structVar))
   {
      printf("P1:Thread2 create error : %s\n",strerror(errno));
      return EXIT_FAILURE;
   }
   message("p1--->thread 2 success");
   
   /*
         wait for the thread to exit
   */

   /***************************************************************************************/

   
   /*******************************join threads *******************************************/
   handleJoin1 = pthread_join(Thread1,NULL);
   if(0!= handleJoin1)
   	{
   		printf("p1:Thread1 join error : %s\n",strerror(handleJoin1));
      return EXIT_FAILURE;
   	
   	}
   chkret("p1T1 handlejoin", handleJoin1);
   handleJoin2 = pthread_join(Thread2,NULL);
   
   if(0!= handleJoin2)
	   {
		   printf("p1:Thread1 join error : %s\n",strerror(handleJoin2));
		 return EXIT_FAILURE;
	   
	   }
   chkret("p1T2 handlejoin", handleJoin2);
/*******************************************************************************************/
//   print();
   printf("\np1:-Var1 = %d\np1:-Var2 = %d\np1:-Var3 = %d\n",structVar->Var1,structVar->Var2,structVar->Var3);
	sempost =sem_post(structVar->semLock);
	chkret("sempost returns",sempost );
   if(-1 == sem_destroy(&semLock))
   	{
   		perror("sem_destroy");
   	}




   if(-1 == shmctl(SHM_Id2,IPC_RMID,0))
	   {
		   printf("2shmctl Failed\n");
	   }
	  /*
			destroy mutex lock
	  */
	  //pthread_mutex_destroy(&Lock); 
	  shmdt(semLock);






   /*
			after work done remove shm
	  */

	if(-1 == shmctl(SHM_Id,IPC_RMID,0))
	{
		printf("shmctl Failed\n");
	}
   /*
         destroy mutex lock
   */
   pthread_mutex_destroy(&Lock); 
   shmdt(structVar);
   shmdt(semLock);
}





void* incVar(struct var * variable)
{
   int i,lk;
   //variable->P = 'A';
   message("p1INC in incvar");
   for(i=0;i<5;i++)
   {
      /*
            mutex lock
      */
      lk = pthread_mutex_lock(&Lock);
    // chkret("p1INC mutex lock returns",lk);
      variable->Var1++;
      variable->Var2++;
      variable->Var3++;
	//  message("p1INC in mutex unlock");
	lk = pthread_mutex_unlock(&Lock);
      //chkret("p1INC mutex unlock returns",lk);
	  
	}
   return NULL;
}

void* decVar(struct var * variable)
{
   int i,lk;
   message("p1DEC in decvar");
   //variable->P = 'A';
   for(i=0;i<5;i++)
   {
   
			  lk = pthread_mutex_lock(&Lock);
     //chkret("p1DEC mutex lock returns",lk);
	 variable->Var1--;
      variable->Var2--;
      variable->Var3--;
	  lk = pthread_mutex_unlock(&Lock);
		//	chkret("p1INC mutex unlock returns",lk);

    }
    return NULL;
}

