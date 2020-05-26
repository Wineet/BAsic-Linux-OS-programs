/*
 *Author: Vinit
 *Platform: Linux
 *Hardware: R-Pi 3b+
 *Task: Socket Server for different Machine But common netwrok
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

#include<netinet/ip.h>
#include<netinet/in.h>
#include<arpa/inet.h>

 struct {
	int msg_no;
	char str[20];
}soc_payload;


void main()
{

	int soc_fd=0;
	int cl_soc_fd=0;
	struct sockaddr_in soc_address;
	struct sockaddr_in client_soc_address;		
	int cl_len=0;
	int ret_val=0;
	soc_fd = socket(AF_INET,SOCK_STREAM,0);		//Two way communication byte stream IPV4 protocol
	
	if(-1 == soc_fd)
	{
		printf("socket Error %s",strerror(soc_fd));
		exit(EXIT_FAILURE);
	}
	
	soc_address.sin_family = AF_INET;
	soc_address.sin_port   = 7676;			// Added a random Port
	soc_address.sin_addr.s_addr = inet_addr("192.168.43.93");

	unlink("my_inet_socket");
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

	
		cl_soc_fd=accept(soc_fd,(struct sockaddr *)&client_soc_address,(socklen_t *)&cl_len);
		if(-1 == cl_soc_fd)
		{
			printf("accept Error %s\n",strerror(cl_soc_fd));
			exit(EXIT_FAILURE);
		}
	while(1)
	{	
		ret_val=read(cl_soc_fd,&soc_payload,sizeof(soc_payload));
		

		if(-1==ret_val)
		{
			printf("read Error %s\n",strerror(ret_val));
		
		}
		printf("Rec. Msg No.=%d \n msg str =%s\n",soc_payload.msg_no,soc_payload.str);

		if(0==strcmp(soc_payload.str,"end"))
		{
			printf("Server Exititng\n");
			close(cl_soc_fd);
			/*
			 * Need to release attributes
			 *
			 * */
			exit(EXIT_SUCCESS);
		}
	}

	
}
