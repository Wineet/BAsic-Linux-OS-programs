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
     ret_val=   execl("./prog1.o","prog1.o",NULL);
     if(-1 == ret_val)
				{
					printf("execl Failed \n");
				
					exit(EXIT_FAILURE);
					
				}
     
  }
  else{
  printf("process2\n");
  ret_val=execl("./prog2.o","prog2.o",NULL);
  if(-1 == ret_val)
				{
					printf("execl Failed \n");
				
					exit(EXIT_FAILURE);
					
				}
  }
  
  
   return 0;
}