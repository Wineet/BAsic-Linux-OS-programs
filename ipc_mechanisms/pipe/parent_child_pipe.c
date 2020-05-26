#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>

int main(int argc,char* argv[])
{
	int pipe_fd[2]={0};	//Pipe File Descriptor
	int ret_val = 0;
	char read_buf[20]={0};	// buffer for read Function call
	char write_buf[20]={0};
	pid_t child_pid;

/*
	if( argc <= 1 || strlen(argv[1]) > 20 )
	{
		printf("Max Size or no input Exit\n");
		exit(EXIT_FAILURE);
	
	}
*/
	ret_val= pipe(pipe_fd);
	if(ret_val !=0)
	{
		printf("Pipe Call Error= %s\n",strerror(ret_val));
		exit(EXIT_FAILURE);
	}

	child_pid=fork();
	
	if(child_pid == -1)
	{
		printf("Fork Error\n");
		exit(EXIT_FAILURE);
	
	}
	
	if (child_pid != 0)
	{
		/* Parents Loop */
		close(pipe_fd[0]);
		while(1)
		{
			printf("Parent Write\n");
			scanf("%s",write_buf);
			ret_val=write(pipe_fd[1],write_buf,(size_t)20);
	
			if(0 == strcmp("end",write_buf))
			{
				printf("Parent Exit\n");
				close(pipe_fd[1]);
				exit(EXIT_SUCCESS);
			
			}	
			if(-1 == ret_val)
			{
				printf("write Error %s\n",strerror(ret_val));
				exit(EXIT_FAILURE);
			}
		}
	}
	else
	{
		/* Childs loop */
		close(pipe_fd[1]);
		while(1)
		{
			ret_val= read(pipe_fd[0],read_buf,20);
			printf("Child Read String %s \n",read_buf);
	
			if(0==strcmp("end",read_buf))
			{
				printf("Child Exit\n");
				close(pipe_fd[0]);
				exit(EXIT_SUCCESS);
			}
		}
	
	}


	return 0;
}
