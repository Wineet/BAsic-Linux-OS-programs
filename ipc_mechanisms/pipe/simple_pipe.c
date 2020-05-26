#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>

int main(int argc,char* argv[])
{
	int pipe_fd[2]={0};	//Pipe File Descriptor
	int ret_val = 0;
	char read_buf[20]={0};	// buffer for read Function call

	if( argc <= 1 || strlen(argv[1]) > 20 )
	{
		printf("Max Size or no input Exit\n");
		exit(EXIT_FAILURE);
	
	}

	ret_val= pipe(pipe_fd);
	if(ret_val !=0)
	{
		printf("Pipe Call Error= %s\n",strerror(ret_val));
		exit(EXIT_FAILURE);
	}


	ret_val=write(pipe_fd[1],argv[1],(size_t)strlen(argv[1])+1);
	if(-1 == ret_val)
	{
		printf("write Error %s\n",strerror(ret_val));
		exit(EXIT_FAILURE);
	}
	ret_val= read(pipe_fd[0],read_buf,20);
	printf("Read String %s \n",read_buf);

	close(pipe_fd[0]);
	close(pipe_fd[1]);

	return 0;
}
