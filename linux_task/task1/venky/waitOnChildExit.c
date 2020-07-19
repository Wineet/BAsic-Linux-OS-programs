#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

int main()
{
	pid_t ProcessId1,ProcessId2;
	int i = 0;
	int StatFrChild1 =0,StatFrChild2=0;
	ProcessId1 =fork();

	if (ProcessId1 > 0)		
	{						<<Vinit: Parent Will enter into this loop as Fork() call is sucess
									 Process Id will be non Zero>>
		if(ProcessId1 > 0)	<<Vinit: Again Checking The Same Condition Doesn't Make Any Sense>>
		{
		for(i=0;i<5;i++)
		{
			printf("P1: Am Alive\n");	<<Vinit: Parent Will Print This, It should be done by child >>
		}
		exit(0);			<< Vinit: Parent will exit Only>>
		}

	}
	else if(ProcessId1 == 0)	<<Vinit: Child Will Execute This Loop>>
	{
		ProcessId2 = fork();	<<Vinit: Child Will create one more Child* >>
		if(ProcessId2 > 0)
		{						<< Vinit: This will execute by Child>>
			for(i=0;i<5;i++)
			{						

			    printf("P2: Am Alive\n");
			}
			exit(0);			<< Vinit:Child exited Here child* is still present But>>
		}
		else if (ProcessId2 == 0)	<<Vinit: Child* will execute this as it don't have any
											herirachal child it will not wait at any wait pid>>
		{
		//StatFrChild1 = waitpid(ProcessId1,NULL,0);
		//StatFrChild2 = waitpid(ProcessId2,NULL,0);
		wait(NULL);

		wait(NULL);
		printf("bye bye \n");

		}
		
	}
	return 0;
}
