/*
 * Author:   vinit 
 * Task:     Pipe between Parent And Child For Communication
 * Hw  : 	   R-pi 3B+
 * Platform: Linux
 *
 */


#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>

#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/wait.h>
/*Function Declaration Here*/


/*Global Variable Here*/


int  main(int argc,char *argv[])
{

	int pipe_fd[2]={0};	//PipeFd
	if(0 != pipe(pipe_fd)) 	// initilise Pipe Both Ends Read and write
	{
		printf("pipe Failed %s \n",strerror(errno));
		exit(EXIT_FAILURE);
	}	
	/*Creating Child Now*/

		
	if(0 == fork())
	{
		printf("Child Created \n");
		char ip_char[5]={0};
			// Child Waiting for parent Message
		close(pipe_fd[1]);
		if(-1 == read(pipe_fd[0],ip_char,5))	
		{
			printf(" read Error %s \n",strerror(errno));
			exit(EXIT_FAILURE);

		}
		if(!strcmp(ip_char,"GO"))
		{
			for(int i=0;i<5;i++)
			{
				printf("I am Child\n");
			}
		}
		/* In case of Else if string doesn't match here it is Exiting
		 * But If want we can again Wait for input
		 * */
		exit(0);
	}

	sleep(3);	// Kept Parent in sleep to ensure Child is waiting For parent Input
	close(pipe_fd[0]);
	//once i/p is given Child Starts to execute
	if( -1 ==  write(pipe_fd[1],"GO",sizeof("GO")) )	// Expecting Null char to write too
	{
		printf("Pipe write Error %s \n",strerror(errno));
		exit(EXIT_FAILURE);
	
	}


	wait(NULL);	//wait for child to exit
	printf("parent Exit\n");
	return 0;
}
