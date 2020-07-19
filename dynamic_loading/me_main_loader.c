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
#include<dlfcn.h>		// To Find symbol in dynamic and shared file

#define DEBUG 1
 
#if DEBUG == 1
 		#define dbug(arg,...) printf(arg,##__VA_ARGS__)
#else
 		#define dbug(arg,...)  
#endif


/*Function Declaration Here*/


/*Global Variable Here*/


int  main(int argc,char *argv[])
{


	void * handle = NULL;
	
	dbug("File name %s\n",argv[1]);
	
	handle = dlopen(argv[1],RTLD_LAZY);
	if( NULL == handle )
	{
		dbug("dlopen Error %s\n", dlerror() );
		exit(0);		
	}	

	int (*fun_handle)(int ,char **);
	fun_handle = dlsym(handle,"me_main");
	
	if(NULL == fun_handle )
	{
		dbug("dlsym Error %s\n",dlerror() );
		exit(0);
	}
	  (*fun_handle)(0,NULL);
	if ( 0 != dlclose(handle) )
	{
		dbug("dlclose Error %s \n",dlerror() );
		exit(0);
	}
		
return 0;
}
