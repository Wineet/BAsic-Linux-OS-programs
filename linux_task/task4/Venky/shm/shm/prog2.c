#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>


#define KEY 2942U
#define PAGESIZE 16384U

struct var
{

   int Var1;
   int Var2;
   int Var3;
}SHM_Struct;
void decVar(struct var * variable)
{
   int i;
  
   for(i=0;i<65530;i++)
   {
      variable->Var1--;
      variable->Var2--;
      variable->Var3--;
   }
}
void print()
{
   int i;
   for(i=0;i<10;i++)
   {
   printf("process : p2 \n");
   sleep(1);
   }
}
int main()
{
   int SHM_Id = 0U;
   SHM_Id = shmget(KEY,sizeof(SHM_Struct),0666|IPC_CREAT);
   
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
   decVar(structVar);
   //print();
   printf("\np2:-Var1 = %d\np2:-Var2 = %d\np2:-Var3 = %d\n",structVar->Var1,structVar->Var2,structVar->Var3);
   
   shmdt(structVar);
}