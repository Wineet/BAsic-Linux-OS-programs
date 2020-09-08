/*
 * Author:   vinit 
 * Task:     User Process to read and write From Driver,
 * 		depending on input arguments Process will write/read to/from Driver File
 * Hw  : 	   R-pi 3B+
 * Platform: Linux
 *
 */


#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>

#define DEBUG		 0
#define READ_CMD	 "read"
#define WRITE_CMD	 "write"
#define FILE_NAME	 "/dev/scull0"
#define SUCCESS 	 0
#define MAX_BUFF_SIZE	 1024

#if DEBUG == 1
 		#define dbug(arg,...) printf(arg,##__VA_ARGS__)
#else
 		#define dbug(arg,...)  
#endif


/*Function Declaration Here*/
char *read_from_file(char *filp,char *user_buff);
int   write_to_file(char *filp,char *user_buff);

/*Global Variable Here*/


int  main(int argc,char *argv[])
{
	char user_buff[MAX_BUFF_SIZE]={0};
	if( argc != 2 )
	{
		printf("Wrong Arguments arg1: read/write\n");
		exit(1);
	}
	dbug("arg1=%s\n",argv[1]);
	if(!strcmp(argv[1], READ_CMD))
	{
		printf("read:%s\n",read_from_file(FILE_NAME,user_buff));
		exit(0);
	}
	if(!strcmp(argv[1], WRITE_CMD))
	{
		printf("Enter Write Data: \n");
		if(NULL == fgets(user_buff,MAX_BUFF_SIZE,stdin))
		{
			printf("User input Failed");
			exit(1);
		}
		if(SUCCESS == write_to_file(FILE_NAME,user_buff))
		{
			printf("write succesful\n");
			exit(0);
		}
		else
		{
			printf("write Failed\n");
			exit(1);
		}
	}


return 0;
}

char *read_from_file(char *filp,char *user_buff)
{
	FILE *readFile_p = NULL;
	int ret_val=0;
	readFile_p=fopen(filp,"r");
	if(readFile_p == NULL)
	{
		printf("fopen Error %s\n",strerror(errno));
		return NULL;
	}
	ret_val=fread(user_buff,1,MAX_BUFF_SIZE-1,readFile_p);
	dbug("Bytes read =%d Data=%s \n",ret_val,user_buff);

	if(EOF == fclose(readFile_p))
	{
		printf("fclose Error %s\n",strerror(errno));
	}
	return user_buff;

}

int  write_to_file(char *filp,char *user_buff)
{
	FILE *writeFile_p = NULL;
	int ret_val=0;
	writeFile_p=fopen(filp,"w");
	if(writeFile_p == NULL)
	{
		printf("fopen Error %s\n",strerror(errno));
		return -1;
	}
	ret_val=fwrite(user_buff,1,MAX_BUFF_SIZE-1,writeFile_p);
	dbug("Bytes written =%d \n",ret_val);
	if(EOF == fclose(writeFile_p))
	{
		printf("fclose Error %s\n",strerror(errno));
	}
	return SUCCESS;

}
