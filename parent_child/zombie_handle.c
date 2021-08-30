#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>

#include<signal.h>

void ctrl_c_handler(int);

int  main(int argc,char *argv[])
{

	/*
	 * Registering Signal Handler Here
	 *
	 * */
	printf("Parent PID = %d\n",getpid());
	signal(SIGCHLD,ctrl_c_handler);
    int pid=fork();
    if(0==pid)
    {
        printf("Child PID = %d\n",getpid());
    }
	while(0!=pid)
	{
	
		printf("Parent: Hello World\n");
		sleep(1);
	}

/*---- Write code here ---*/


return 0;
}



void ctrl_c_handler(int sig_arg)
{
    int status=0;
	printf("Child Stopped or terminated");

    printf("waitpid =%d \n",waitpid(-1,&status,WNOHANG));
    return;
}