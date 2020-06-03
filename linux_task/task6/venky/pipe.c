#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
enum{FORK_FAILED = -1,PARENT=1,CHILD=0}pros;
#define messege(x,y) printf("%s %s\n",x,y)
void child_process();

void parent_process();
int pfd[2];
int main()
{
   int retval;
//   pid_t ret;
   retval = pipe(pfd);
  if(-1 == retval)
  {
    messege("pipe","error");
  }

switch(fork())
{
   case FORK_FAILED : messege("fork failed..","");
                      exit(0);
   case CHILD : child_process();
                break;
   default : parent_process();
             break;
}

}


void child_process()
{
   char buff[10];
   close(pfd[1]);
   read(pfd[0],buff,10);
   messege("child printing :",buff);
   while(0 != strcmp(buff,"Go"))
   {
   read(pfd[0],buff,10);
   messege("child printing :",buff);
//   sleep(1);
     
   }
   messege("child printing :","i am child");
   messege("child printing :","i am child");
   messege("child printing :","i am child");
   messege("child printing :","i am child");
   messege("child printing :","i am child");
   messege("child","exiting");
   exit(0);
}

void parent_process()
{
  char buff[10];
   int retval;
  close(pfd[0]);
  messege("enter input","|| enter \" Go\" to exit parent and child ");
  scanf("%s",buff);

   while(0 != strcmp(buff,"Go"))
   {
      write(pfd[1],buff,10);
  messege("enter input","|| enter \" Go\" to exit parent and child ");
      scanf("%s",buff);
   }
      write(pfd[1],buff,10);

  wait(&retval);
messege("parent","exiting");

}
