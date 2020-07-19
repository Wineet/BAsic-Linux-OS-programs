/*
 * Author:   vinit 
 * Task:     Register Linux OS signal to avoide terminal Exit  if some one press "Ctrl+c"
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

#include<signal.h>

#define DEBUG 1
 
#if DEBUG == 1
 		#define dbug(arg,...) printf(arg,##__VA_ARGS__)
#else
 		#define dbug(arg,...)  
#endif



/*Function Declaration Here*/
void ctrl_c_handler(int);

/*Global Variable Here*/


int  main(int argc,char *argv[])
{

	/*
	 * Registering Signal Handler Here
	 *
	 * */
	printf("PID = %d\n",getpid());
	signal(SIGINT,ctrl_c_handler);
	while(1)
	{
	
		printf("Hello World\n");
		sleep(1);
	}

/*---- Write code here ---*/


return 0;
}



void ctrl_c_handler(int sig_arg)
{

	printf("CTRL+C\n");

}

