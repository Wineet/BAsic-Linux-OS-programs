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


void main()
{

	int ret_val=0;
	int file_fd=0;
	char read_buf[20]={0};
	
	if( 0 != access("/tmp/fifo_pipe",F_OK))	// Checking for File Existance
	{

		ret_val=mkfifo("/tmp/fifo_pipe",(mode_t)0777);	//Creating FIFO name: fifo_pipe permission 0777
		if(0 != ret_val )
		{
			printf("fifo Creat Failed %s\n",strerror(ret_val));
			exit(EXIT_FAILURE);
		}
	}

	while(1)
	{
		
		file_fd=open("/tmp/fifo_pipe",O_RDONLY);	//Process is blocked until someone ones FIFO for write
		if(-1 == file_fd)
		{
			printf("OPEN Error %s \n",strerror(file_fd));
			exit(EXIT_FAILURE);
		}
		ret_val=read(file_fd,read_buf,20);		// Max Size is 20 in our case
								// Read is a blockable API
		if(-1 == ret_val)
		{
			printf("Read Fail %s\n",strerror(ret_val));
			exit(EXIT_FAILURE);
		}
	
		printf("Read String=%s\n",read_buf);
		
		if(0== strcmp("end",read_buf))
		{
			printf("Program End\n");
			exit(EXIT_SUCCESS);
		}	

		memset(read_buf,0,20);	//clear buffer
	
	
		close(file_fd);

	}
return ;
}
