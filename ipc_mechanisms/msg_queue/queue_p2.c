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
	

	int msg_queue_id=0;
	int ret_val = 0;
	struct payload_msg buffer;
	int value=0;

	msg_queue_id=msgget(IPC_QUEUE_KEY, IPC_CREAT|0777);	// Message queue created with permission
	if(-1==msg_queue_id)
	{
		printf("msgget Failed %s\n",strerror(msg_queue_id));
		exit(EXIT_FAILURE);
	
	}

	ret_val=msgrcv(msg_queue_id,&buffer,(size_t)20,0,0);	// Here We are reading First message from queue
	
	if(-1 == ret_val)
	{
		printf("msgsnd Failed %s\n",strerror(ret_val));
		exit(EXIT_FAILURE);
	}
	
	printf("received message=%s \n",buffer.arr);

	if(0==strcmp(buffer.arr,"end"))	
	{
		ret_val=msgctl(msg_queue_id,IPC_RMID,0);			// removes message queue IPC	
		if(-1 == ret_val)
		{
			printf("msgctl Error %s\n",strerror(ret_val));
			exit(EXIT_FAILURE);
		}
	}
return;
}
