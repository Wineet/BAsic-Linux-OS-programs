#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include<sys/wait.h>

#define KEY 2942U
#define PAGESIZE 16384U

struct var
{
	//int P;
   int Var1;
   int Var2;
   int Var3;
}SHM_Struct;
void incVar(struct var * variable)
{
   int i;
   //variable->P = 'A';
   for(i=0;i<65530;i++)
   {
      variable->Var1++;
      variable->Var2++;
      variable->Var3++;
   }
}

void print()
{
   int i;
   for(i=0;i<10;i++)
   {
   printf("process : p1 \n");
   sleep(1);
   }
}
int main()
{
   int SHM_Id = 0U;
   SHM_Id = shmget(KEY,sizeof(SHM_Struct),0666|IPC_CREAT); // read write execute permission 
   
   if(-1 == SHM_Id)
   {
      perror("shmget failed");
   }
   //   void *shmat(int shmid, const void *shmaddr, int shmflg);
   struct var *structVar = shmat(SHM_Id,NULL,0666);
   if ((void*)-1 == structVar)
   {
      perror("shmat failed");
   }
   incVar(structVar);
//   print();
   printf("\np1:-Var1 = %d\np1:-Var2 = %d\np1:-Var3 = %d\n",structVar->Var1,structVar->Var2,structVar->Var3);

   shmdt(structVar);
}