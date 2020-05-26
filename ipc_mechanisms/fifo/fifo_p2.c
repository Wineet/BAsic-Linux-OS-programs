/*
 * FIFO Code 
 * Author : Vinit
 * Machine: R-pi 3B+
 * */

#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>


void main(int argc,char *argv[])
{

	int ret_val=0;
	int file_fd=0;
	//char read_buf[20]={0};
	if(1 == argc || (strlen(argv[1]) > 20) )
	{
		printf("No Argument or Max Size exceed\n");
		exit(EXIT_FAILURE);
	
	}

	if( 0 != access("/tmp/fifo_pipe",F_OK))	// Checking for File Existance
	{

		ret_val=mkfifo("/tmp/fifo_pipe",(mode_t)0777);	//Creating FIFO name: fifo_pipe permission 0777
		if(0 != ret_val )
		{
			printf("fifo Creat Failed %s\n",strerror(ret_val));
			exit(EXIT_FAILURE);
		}
	}

	file_fd=open("/tmp/fifo_pipe",O_WRONLY);	// Process is blocked until someone opens FIFO for Write
	if(-1 == file_fd)
	{
		printf("Open Error=%s\n",strerror(file_fd));
		exit(EXIT_FAILURE);

	}

	
		ret_val=write(file_fd,argv[1],20);		// Max Size is 20 in our case
								// Read is a blockable API
		if(-1 == ret_val)
		{
			printf("Read Fail %s\n",strerror(ret_val));
			exit(EXIT_FAILURE);
		}
	
	
	close(file_fd);


return ;
}
