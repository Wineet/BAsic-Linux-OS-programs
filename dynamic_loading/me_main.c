/*
 * Author:   vinit 
 * Task:     --Here--
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

#define DEBUG 1
 
#if DEBUG == 1
 		#define dbug(arg,...) printf(arg,##__VA_ARGS__)
#else
 		#define dbug(arg,...)  
#endif


/*Function Declaration Here*/


/*Global Variable Here*/


void  me_main(int argc,char *argv[])
{
	
		dbug("me_main Loaded\n");
	


return 0;
}
