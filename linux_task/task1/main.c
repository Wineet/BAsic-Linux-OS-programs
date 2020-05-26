/*
 *Author : vinit
 *platform: Linux
 *Task : Fork,execl Demo
*/


#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

#define TRUE 1
#define FALSE 0
void main()
{

	pid_t child1_pid=0;
	pid_t child2_pid=0;
	pid_t buffer_pid =0;
	int ret_val=0;
	int status =0;

	for(int i=0;i<2;i++)
	{
		buffer_pid=fork();
		if(-1 == buffer_pid)
		{
			printf("Fork Failed %s\n",strerror(buffer_pid));
			/*Need to place kill signal here if i==1 one process if already created*/
			exit(EXIT_FAILURE);
		
		}
		if(0 == buffer_pid)
		{
			if(0 == i)
			{
				child1_pid = buffer_pid;
				/*Need to do execl here only*/
				ret_val = execl("./t1","t1",NULL);
				if(-1 == ret_val)
				{
					printf("execl Failed \n");
				/*Need to kill created Process */
					kill(child1_pid,9);		//Only one process created so one Kill
					exit(EXIT_FAILURE);
					
				}
				break;
			}				
			if(1 == i)
			{
				child2_pid = buffer_pid;
				/*Need to do execl here only*/
				ret_val = execl("./t2","t2",NULL);
				if(-1 == ret_val)
				{
					printf("execl failed\n");
				/*Need to kill created Process*/
					kill(child1_pid,9);
					kill(child2_pid,9);		//Kill signal to created Process
					exit(EXIT_FAILURE);
				
				}
				break;
			}
		}
	}


/*Wait PID Here*/

	for(int i=0;i<2;i++)
	{
		buffer_pid=waitpid(-1,&status,0);		// by default wait pid wait for termination of child
		
/* With PID Value you can make better
*  error report generation that which child
*  has exited abnormally
*
*/
		  if(TRUE == WIFEXITED(status))
			{
				printf("Child Exited Normally\n");
			}
		
	}
	

printf("Bye Bye...\n");

return;
}
