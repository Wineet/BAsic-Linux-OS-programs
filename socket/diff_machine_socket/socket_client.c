/*
 *Author: Vinit
 *Platform: Linux
 *Hardware: R-Pi 3b+
 *Task: Socket client with Different Netwrok:
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

//#define ARG_ACTIVE

 struct {
	int msg_no;
	char str[20];
}soc_payload;


void main(int argc, char *argv[])
{

#ifdef ARG_ACTIVE
	if(1==argc|| ( strlen(argv[1]) >20 ) )
	{
		printf("No input or max count exceed\n");
		exit(EXIT_FAILURE);
	
	}
#endif
	int soc_fd=0;
	int cl_soc_fd=0;
	struct sockaddr_in soc_address;
	struct sockaddr_in client_soc_address;
	int cl_len=0;
	int ret_val=0;
	soc_fd = socket(AF_INET,SOCK_STREAM,0);	//Two way communication byte stream
	
	if(-1 == soc_fd)
	{
		printf("socket Error %s",strerror(soc_fd));
		exit(EXIT_FAILURE);
	}
	
	soc_address.sin_family = AF_INET;
	soc_address.sin_port   = 7676;
	soc_address.sin_addr.s_addr = inet_addr("192.168.43.93");

	ret_val=connect(soc_fd,(struct sockaddr *)&soc_address,sizeof(soc_address));
	if(-1 == ret_val)
	{
		printf("connect Error %s\n",strerror(ret_val));
		exit(EXIT_FAILURE);
	
	}
	char buff[20];
while(1)
{
	soc_payload.msg_no=1;
	printf("Enter Payload to send\n");
	scanf("%s",buff);
	strcpy(soc_payload.str,buff);
	ret_val=write(soc_fd,&soc_payload,sizeof(soc_payload));
		if(-1==ret_val)
		{
			printf("Write Error %s\n",strerror(ret_val));
		
		}
}		
		close(soc_fd);
			exit(EXIT_SUCCESS);
		
}
