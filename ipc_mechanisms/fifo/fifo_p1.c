/*
 * FIFO Code 
 * Author : Vinit
 * Machine: R-pi 3B+
 * */

/* Program Note
 *
 *
 

How to do continues reading in FIFO

When you launch both the program the "read" and "write" end of a FIFO is opened, 
as you do the message transaction by writing and reading it works fine, 
read gets blocked until some program writes to it.
as soon as you terminate the writing program you have closed one end of FIFO and the connection broke.
so Now "read" will every time read EOF and return 0, you will end up in non-blocking of read case.
In your case you are not clearing buffer so Old value will be printed.
you can open and close connection for every Message so even if you close the "write" process abruptly,
the next open call of the "read" process will get blocked until some process opens "write" end.

*
*/
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
		printf("Before Open \n");
		file_fd=open("/tmp/fifo_pipe",O_RDONLY);	//Process is blocked until someone ones FIFO for write
		if(-1 == file_fd)
		{
			printf("OPEN Error %s \n",strerror(file_fd));
			exit(EXIT_FAILURE);
		}
		printf("After Open \n");
		ret_val=read(file_fd,read_buf,20);		// Max Size is 20 in our case
								// Read is a blockable API
		if(-1 == ret_val)
		{
			printf("Read Fail %s\n",strerror(ret_val));
			exit(EXIT_FAILURE);
		}
		printf("After read \n");
		printf("Ret value %d ",ret_val);
		printf("Read String=%s\n",read_buf);
		
		if(0== strcmp("end",read_buf))
		{
			printf("Program End\n");
			exit(EXIT_SUCCESS);
		}	
		memset(read_buf,0,20);	//clear buffer
		sleep(1);
		close(file_fd);
	}
return ;
}
