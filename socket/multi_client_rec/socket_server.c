/*
 *Author: Vinit
 *Platform: Linux
 *Hardware: R-Pi 3b+
 *Task: Socket Server
 *
 */


#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<unistd.h>
#include<pthread.h>

 typedef struct {
	int msg_no;
	char str[20];
}sock_payload;

typedef struct{
	
	int msg_no;
	int client_id;
	char str[20];

}pipe_payload;

int pipe_fd[2]={0};


void *read_thread(void *);
void init_comm();
pipe_payload*	get_message();
void	send_message(pipe_payload);
void* server_action_thread(void *);

void main()
{
		
	int soc_fd=0;
	int cl_soc_fd=0;
	struct sockaddr_un soc_address;
	struct sockaddr_un client_soc_address;
	int cl_len=0;
	int ret_val=0;

	pthread_t thread_id=0;
	pthread_t server_thread=0;
	pthread_t thread_id_arr[20]={0};		// For now taking static array to store thread_id
	int cl_soc_fd_arr[20]={0};			// For 20 Clients we are creating		
	
	init_comm();				// Create Pipe to form Communication

	soc_fd = socket(AF_UNIX,SOCK_STREAM,0);	//Two way communication byte stream
	
	if(-1 == soc_fd)
	{
		printf("socket Error %s",strerror(soc_fd));
		exit(EXIT_FAILURE);
	}
	
	soc_address.sun_family = AF_UNIX;
	strcpy(soc_address.sun_path,"my_socket");
	unlink("my_socket");
	ret_val=bind(soc_fd,(struct sockaddr *)&soc_address,sizeof(soc_address));
	if(0 != ret_val)
	{
		printf("bind Error %s\n",strerror(ret_val));
		exit(EXIT_FAILURE);
	
	}
	ret_val=listen(soc_fd,10);	// Queue Size 10 ready to receive msg 10 stands for number of pending connection in queue
	if(0 != ret_val)
	{
		printf(" Listen Error %s\n",strerror(ret_val));
		exit(EXIT_FAILURE);
	
	}

	int thread_id_flag=0;

	pthread_create(&server_thread,NULL,&server_action_thread,NULL); 	// Creating main server thread	
	while(1)
	{
		cl_soc_fd=accept(soc_fd,(struct sockaddr *)&client_soc_address,(socklen_t *)&cl_len);
		if(-1 == cl_soc_fd)
		{
			printf("accept Error %s\n",strerror(cl_soc_fd));
			/*
			 * Exception needs to be handled Only Exit will not work it may lead to unwanted behaviour
			 *
			 * */
			//	exit(EXIT_FAILURE);
		}	
		
		ret_val=pthread_create(&thread_id,NULL,&read_thread,&cl_soc_fd);
		if(0 != ret_val)
		{
			printf("pthread_create Error %s\n",strerror(ret_val));
			/*Complete Exit Can't Do*/
		}
		
		thread_id_arr[thread_id_flag++] = thread_id;	// It is useful at end to perform thread_join
		/*After Every connection we need to create thread and paassing argument as client_id*/
 	
	}

	
}



void *read_thread(void *arg)		// All Clients will be performing this re-entrent thread
{

	int ret_val  = 0;
	int cl_soc_fd= 0;
	pipe_payload read_msg;
	int *ptr = (int *)arg;
	cl_soc_fd=*ptr;
	
	sock_payload soc_payload;
	while(1)
	{
		ret_val=read(cl_soc_fd,&soc_payload,sizeof(soc_payload));	
		if(-1==ret_val)
		{
			printf("Read Error %s\n",strerror(ret_val));
			/*
			 * Pthread exit or some exception handle need to implement*/
		}
	// Added Below Line for debugging
	//	printf("client_id=%d, payload=%s\n",cl_soc_fd,soc_payload.str);
		
		if(0==strcmp(soc_payload.str,"end"))
		{
			close(cl_soc_fd);
			pthread_exit(&ret_val);			//if client send end no need reading thread
		}
		else
		{
			read_msg.msg_no = 1;
			read_msg.client_id=cl_soc_fd;
			strcpy( read_msg.str, soc_payload.str);

			send_message(read_msg);
		}
		/*
		 *
		 *	post message to pipe Here which will received by server to process data
		 * 	Client will be receiving data by socket Only
		 * */
	}


return (void *)NULL;
}


/*
 * Server main Action thread
 *
 *
 * */

void* server_action_thread(void *arg)
{
	pipe_payload buff;
	while(1)
	{
		read(pipe_fd[0],&buff,sizeof(pipe_payload));
		printf("Data From Client \n client_id=%d,payload=%s\n",buff.client_id,buff.str);

	}

return (void *)NULL;
}


/*
 *  Pipe Function Call
 *  For process and thread communication
 * */

void  init_comm()
{
	int ret_val=0;  
	ret_val=pipe(pipe_fd);
	if(0 != ret_val)
	{
		printf("Pipe Error %s\n",strerror(ret_val));
	
	}
return;
}

void send_message(pipe_payload  send_message)
{
	int ret_val=0;
	ret_val=write(pipe_fd[1],&send_message,sizeof(pipe_payload));
	if(-1 == ret_val )
	{
		printf("Write Error %s\n",strerror(ret_val));

	}
return;
}

pipe_payload*  get_message()
{
	static pipe_payload get_msg;
	int ret_val=0;
	ret_val=read(pipe_fd[0],&get_msg,sizeof(pipe_payload));
	if(-1 == ret_val)
	{
		printf("Read Error\n");
	}

return &get_msg;
}

