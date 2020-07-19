#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ipc.h>   // for ipc
#include <sys/shm.h>   //for shared  mem
#include<sys/wait.h>
 #include <pthread.h> // for creating thread
#include <string.h>

#include<semaphore.h>
#include<unistd.h> //for sleep
#define KEY 2942U
#define PAGESIZE 16384U

pthread_mutex_t  Lock;
struct var
{
   sem_t semLock;
	//int P;
   int Var1;
   int Var2;
   int Var3;
}SHM_Struct;
void* incVar(struct var * variable);

void* decVar(struct var * variable);

void print()
{
   int i;
   for(i=0;i<10;i++)
   {
   printf("process : p2 \n");
   sleep(1);
   }
}
struct var *structVar;

int main()
{
   int SHM_Id = 0U;
   pthread_t Thread1,Thread2;
   /*
         creat shared memory
   */
   SHM_Id = shmget(KEY,sizeof(SHM_Struct),0666|IPC_CREAT); // read write execute permission 
   
   if(-1 == SHM_Id)
   {
      perror("shmget failed");
   }
   //   void *shmat(int shmid, const void *shmaddr, int shmflg);
   /*
         attach shared memory
   */
   structVar = shmat(SHM_Id,NULL,0666);
  //printf("\np2:-Var1 = %d\np2:-Var2 = %d\np2:-Var3 = %d\n",structVar->Var1,structVar->Var2,structVar->Var3);

  
   if ((void*)-1 == structVar)
   {
      perror("shmat failed");
   }

   
   if(-1==sem_init(&(structVar->semLock),1,0))
	 {
	   
		 perror("sem_init failed");
	 }
   
   /*
         create thread
   */
   
   if (0 != pthread_create(&Thread1,NULL,(void*)incVar,(void*)structVar))
   {
      printf("P2:Thread1 create error : %s\n",strerror(errno));
      return EXIT_FAILURE;
   }
   if (0 != pthread_create(&Thread2,NULL,(void*)decVar,(void*)structVar))
   {
      printf("P2:Thread2 create error : %s\n",strerror(errno));
      return EXIT_FAILURE;
   }
   
   /*
         wait for the thread to exit
   */
   pthread_join(Thread1,NULL);
   pthread_join(Thread2,NULL);
//   print();
   printf("\np2:-Var1 = %d\np2:-Var2 = %d\np2:-Var3 = %d\n",structVar->Var1,structVar->Var2,structVar->Var3);

   if(-1 == sem_destroy(&structVar->semLock))
   	{
   		perror("sem_destroy");
   	}


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
}



void* incVar(struct var * variable)
{
   int i;
   //variable->P = 'A';
   for(i=0;i<65530;i++)
   {
      /*
            mutex lock
      */
      if(-1==sem_wait(&structVar->semLock))
      	{
      		perror("sem_wait");
      	}
      pthread_mutex_lock(&Lock);
      variable->Var1++;
      variable->Var2++;
      variable->Var3++;
      pthread_mutex_unlock(&Lock);
	  if(-1 == sem_post(&structVar->semLock))
	  	{
	  		perror("sem_post");
	  	}
   }
   return NULL;
}

void* decVar(struct var * variable)
{
   int i;
   //variable->P = 'A';
   for(i=0;i<65530;i++)
   {
   	if(-1==sem_wait(&structVar->semLock))
      	{
      		perror("sem_wait");
      	}      pthread_mutex_lock(&Lock);
      variable->Var1--;
      variable->Var2--;
      variable->Var3--;
      pthread_mutex_unlock(&Lock);
	  if(-1 == sem_post(&structVar->semLock))
	  	{
	  		perror("sem_post");
	  	}
   }
    return NULL;
}


