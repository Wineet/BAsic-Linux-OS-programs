/*
 * Author: Vinit
 * code:  Linux message queue
 * Platform: R-pi 3B+
 *
 * */


#include<stdlib.h>
#include<stdio.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<string.h>

#define IPC_QUEUE_KEY 8000 		// IPC keys can be found by system function

struct payload_msg{
	long int mtype;			// This should be always greater than zero
					// mtype used at receiver end
	char arr[20];
};

void main(int argc, char *argv[])
{
	
	int value=0;

	if(1 == argc || ( strlen(argv[1]) > 20) )
	{
		printf("NO Argument or Max size exceed\n");
		exit(EXIT_FAILURE);
	
	}
	int msg_queue_id=0;
	int ret_val = 0;
	struct payload_msg buffer;

	msg_queue_id=msgget(IPC_QUEUE_KEY, IPC_CREAT|0777);	// Message queue created with permission
	if(-1==msg_queue_id)
	{
		printf("msgget Failed %s\n",strerror(msg_queue_id));
		exit(EXIT_FAILURE);
	
	}
	buffer.mtype = 1;
	memset(buffer.arr,0,20);		// Clear array
	strcpy(buffer.arr,argv[1]);		// array copy
	

	ret_val=msgsnd(msg_queue_id,&buffer,(size_t)20,0);
	if(-1 == ret_val)
	{
		printf("msgsnd Failed %s\n",strerror(ret_val));
		exit(EXIT_FAILURE);
	}
		
		
return;
}
