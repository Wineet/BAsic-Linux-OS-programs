/*
 * Author:   Vinit
 *  
 * Task:     Design Embedded system for Sensor based door Bell 
 * 		system should be Rigid and reliable and easy to detect the Problems 
 * Hw  : 	   R-pi 3B+
 * Platform: Linux
 *
 *
 */


/**********************************************************
 * Program contains Four threads			  
 * 
 * Thread1 >>		  
 * 		Motion detector Thread 
 * 		Periodic Polling Will be there To detect the Motion
 * 		If Any thing Goes Wrong Will Update Status Variable
 *
 * 				              | |
 * 	                                      | | Thread 1 and thread 2 communicate With Pipe IPC channel
 * 	                                      | |
 * Thread2 >>						  		( Not Used Queue Because No Continues Request can Occure )
 *		Distance calculator Thread 
 *		Once Trigger Received Distance will be read From Device/File
 * 		If Any thing Goes Wrong Will Update Status Variable
 *
 *					      | |
 *					      | | Thread 2 and Thread 3 communicates with Pipe IPC Channel		  
 *					      |	|	  
 * Thread3 >>						  
 *		Door Bell Thread			  
 *		holds 2.5 sec timer forDoor Bell ring then update the status
 * 		If Any thing Goes Wrong Will Update Status Variable
 *		On sucess case also it will trigger thread 4
 *							  
 * Thread4 >>						  
 * 		Takes Decision of sucessful and Failure Case
 *						  	  
 *
 *				Thread 4
 *				 ||	  
 *				 ||  		(Only One Queue is Maintained Between threads)
 *			  _____|____|_____	  
 *			T1	 T2	  T3   (thread 4 Will read From Queue T1 and T2 and T3 write in Queue)
 *							  
 * ********************************************************
 */

#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>

#define _GNU_SOURCE

#include <pthread.h>
#include <limits.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "door_bell.h"
/* Global Macro Definiton Here */

#define TRUE  1
#define FALSE 0
#define PIPE_READ 0
#define PIPE_WRITE 1
#define MAX_MESSAGE_SIZE 20


#define DEBUG 1
 
#if DEBUG == 1
 		#define dbug(arg,...) printf(arg,##__VA_ARGS__)
#else
 		#define dbug(arg,...)  


#endif



/*Global Variable Here*/

static int g_monitorThread_running  =  FALSE;
static int g_motionThread_running   =  FALSE;
static int g_distanceThread_running =  FALSE;
static int g_doorbellThread_running =  FALSE;




/*
 * Resource related Data
 *
 * */
static  int pipe_t1[2]={0};		// Motion Thread Pipe		// For Motion thread can be Config As Non Blocking
static	int pipe_t2[2]={0};		// Distance Thread Pipe
static	int pipe_t3[2]={0};		// Door Bell Thread Pipe

static  key_t const msgQueKey = 2000 ;  // Message queue Key
static  int msgQ_Id = 0;


/*Function Declaration Here*/

void* motion_detector_thread(void *);
void* distance_calculator_thread(void *);
void* door_bell_thread(void *);
void* monitor_thread(void *);

read_data get_value(char *);

void shutdown(void);			// Closes all Open Resources

status_t post_messageToQ(event_data );
status_t get_messageFromQ(event_data *);
status_t read_event(int * , char * , int );
status_t post_event(int *,char * , int);
status_t check_thread_alive(void);

event str_enum( char * );

int  main(int argc,char *argv[])
{

/* this array Keeps Track Of all created Threads
 *
 * */

    pthread_t thread_id[4] = {0};			
    thread_obj  thread_list[4]  = {
	    			{ "monitor_thread"	, 0 , monitor_thread 			},
				{ "motion_thread"	, 0 , motion_detector_thread		},
				{ "Dist_thread" 	, 0 , distance_calculator_thread	},
				{ "door_b_Thread"	, 0 , door_bell_thread			}
   			  };
  

#ifdef MORE_DEBUGGING
    for(int i=0;i<4;i++)
    {
      dbug(" threadName %d=%s\n",i,thread_list[i].threadName);
    
    }
#endif

    dbug(" PIPE_BUF=%ld \n",PIPE_BUF);
/*
 * Creating Resources Here
 * pipe and Queue creation
 * Initilisation Global Variables
 *
 * */

	if (-1 ==  pipe(pipe_t1) )
	{
		printf("Pipe_t1 pipe Error %s\n",strerror(errno) );
		exit(1);
	}
	if (-1 ==  pipe(pipe_t2) )
	{
		printf("Pipe_t2 pipe Error %s\n",strerror(errno) );
		exit(1);
	}
	if (-1 ==  pipe(pipe_t3) )
	{
		printf("Pipe_t3 pipe Error %s\n",strerror(errno) );
		exit(1);
	}

	 system("ipcrm -Q 2000");	// remove IPC key For Queue
	// Need to improve this

	if(-1 ==(msgQ_Id = msgget(msgQueKey,IPC_CREAT|0777) ) )
	{
		printf("Message Queue Creation Failed\n");
		exit(1);
	}
/* While Creating thread Taking Default attribute
 * Passing thread Name as argument
 * First starting Monitor Thread
 * 
 * Every thread is Detached Thread So resources will be automatically
 * Cleared
 *
 * */


    for(int i=0;i<4;i++)
    {
	if( 0 != pthread_create( &thread_list[i].thread_id, NULL , thread_list[i].thread_routine, thread_list[i].threadName ) )
	{
		printf("Pthread Create Failed %s \n",strerror(errno));
		exit(1);
	}
	else
	{
		if( 0 != pthread_detach(thread_list[i].thread_id))
		{
			printf("pthread_detach Fail Resoucr Leak may happen %s \n",strerror(errno));
		/*
		 * Exit Not Necessary As Functionality Will Not Hamper
		 *
		 * */	
		}



		dbug("thread\"%d\" Created =%s\n",i,thread_list[i].threadName);
	}	
    }
#if 0
    for now Commented
 g_monitorThread_running  = TRUE;
 g_motionThread_running   = TRUE;
 g_distanceThread_running = TRUE;
 g_doorbellThread_running = TRUE;
#endif
 g_distanceThread_running = TRUE;
 g_monitorThread_running  = TRUE;
 g_motionThread_running   = TRUE;
 g_doorbellThread_running = TRUE;
 

pthread_exit(0);
return 0;
}



void* motion_detector_thread(void *thread_name)
{
	char *arg_name= (char *)thread_name;
	char msg_buff[ MAX_MESSAGE_SIZE ] = {0};
	event_data event_motion ={0};

	dbug("I am %s\n",arg_name);
	if(0 != pthread_setname_np(pthread_self(),arg_name) )
	{
			printf("warn: Thread Name Not Set\n");
	}
	
	sleep(1);
			


	while( TRUE == g_motionThread_running )
	{
dbug("Motion_");	
		/* Basic Logic of thread
		 * Poll Motion After 2sec Period
		 * If Motion Detected Trigger Distance thread
		 * 
		 * */

	     	if( FAIL == read_event(pipe_t1,msg_buff,MAX_MESSAGE_SIZE ) )
	     	{
	     			printf("Md: read Error\n");
	    	}
	     	dbug("Md: Message from Monitor Thread= %s\n",msg_buff);
		
		switch(str_enum(msg_buff))
		{
			case MOTION_SCAN_ON:
			{
			        dbug("Md: MOTION_SCAN_ON event \n");	
        	 		while(TRUE)
				{
					read_data motion_data = get_value("file/motionSensor.txt");	
	 				dbug("motion value = %d motion status = %d\n",motion_data.value, motion_data.status);	
	         			sleep(2);		 
		 			if(1 == motion_data.value )
		 			{
		 				/*
				 		 * Motion Detected
				 		 * Inform Monitor Thread that Motion Detected
				 		 * Post Message to Queue
				 		 * Wait for Next trigger to start polling again
						 * Trigger Will Come from Monitor Thread
						 */
     	     					  event_motion.msg_data.event = MOTION_DETECTED;
	     			
					  	  if(FAIL == post_messageToQ(event_motion))
	     					  {
	     						printf("Md: Post Message to Q Failed");
	     					  }
						
						break;
		 			}	
				}
			break;	
			}
		
			default:
			{
			 printf("Invalid Event In Motion Thread %d\n",str_enum(msg_buff) );
			}
		
		
		}	
	  }

	pthread_exit(NULL);

 return NULL;
}

void* distance_calculator_thread(void *thread_name)
{
	char *arg_name= (char *)thread_name;
	dbug("I am %s\n",arg_name);
	
	if(0 != pthread_setname_np(pthread_self(),arg_name) )
	{
			printf("Dt: warn: Thread Name Not Set\n");
	}
	sleep(1);

	char msg_buff[MAX_MESSAGE_SIZE]={0};
	event_data event_dist={0};
 
	while( TRUE == g_distanceThread_running )
	{
		memset(msg_buff,0,MAX_MESSAGE_SIZE);	//clearing Buffer

	    if(FAIL == read_event( pipe_t2, msg_buff, MAX_MESSAGE_SIZE ) )
	     {
	     	printf("Dt: read Error\n");
	     }
	     dbug("Dt: Message from Monitor Thread= %s\n",msg_buff);
	
		switch( str_enum(msg_buff) )
		{
			case CALC_DISTANCE:
			{
				/*
				 * Read Distance 
				 * If Distance is Invalid/Valid let know distance is valid
				 *
				 * */
				dbug("%s = CALC_DISTANCE EVENT\n",thread_name);
     	     			//event_dist.msg_data.event = INVALID_DISTANCE;
				
     	     			event_dist.msg_data.event = VALID_DISTANCE;
	     			
				if(FAIL == post_messageToQ(event_dist))
	     			{
	     				printf("Dt: Post Message to Q Failed");
	     			}
			
				/*Post Back value to queue*/
			break;	
			}
			
			case /*Event*/2:
			{
			
				/*post BackValue to queue */
			break;	
			}
			default:
			printf("Dt:Invalid Event in Distance \n");
		
		}




	}


	pthread_exit(NULL);


return NULL;
}

void* door_bell_thread(void *thread_name)
{
	char *arg_name= (char *)thread_name;
	dbug("I am %s\n",arg_name);

	if(0 != pthread_setname_np(pthread_self(),arg_name) )
	{
			printf("warn: Thread Name Not Set\n");
	}
	sleep(1);

	char msg_buff[MAX_MESSAGE_SIZE] = {0};
	while( TRUE == g_doorbellThread_running )
	{
	
	     if(-1 == read(pipe_t3[PIPE_READ],msg_buff,MAX_MESSAGE_SIZE ) )
	     {
	     	printf("Dt: read Error\n");
	     
	     }
	     dbug("Db: Message from Monitor Thread= %s\n",msg_buff);
		
		/*
		 *  Read From Pipe
		 * */

		switch( str_enum(msg_buff) )
		{
			case RING_BELL:
			{
				dbug("Door Bell: RING_BELL \n");
				/*
				 * Start 2.5 Second Timer For Door Bell Ring
				 * It will post Event Back to Same thread 
				 * */


				/*Post Back value to queue*/
			break;	
			}
			
			case RING_BELL_TIMER_EXPIRY:
			{
				dbug("Door Bell: RING_BELL_TIMER_EXPIRY \n");
				/*Post Back value to queue*/	
			break;	
			}
			default:
			printf("Db:Invalid Event in Door Bell \n");
		
		}
	




	}
	pthread_exit(NULL);


return NULL;
}
void* monitor_thread(void *thread_name)
{
	char *arg_name= (char *)thread_name;
	char msg_buff[20]={0};
	event_data event_monitor ={0};

	dbug("I am %s\n",arg_name);

	if(0 != pthread_setname_np(pthread_self(),arg_name) )
	{
			printf("warn: Thread Name Not Set\n");
	}
	sleep(2);	// Value 2 is Imp For thread Sync

	strncpy(msg_buff,"MOTION_SCAN_ON",MAX_MESSAGE_SIZE);
	/*Informing Event to Distance Calculate  Thread */
	if( FAIL == post_event( pipe_t1 , msg_buff, MAX_MESSAGE_SIZE ) )
	{
		printf("Mt: pipe Write Error %s\n",strerror(errno));
	}
//	dbug("%s event %s posted\n",thread_name,msg_buff);
	
	while( TRUE == g_monitorThread_running )
	{

		if (FAIL == get_messageFromQ(&event_monitor) )	// Always read First message of Queue
		{
			printf("Mt: warn: get Message From Queue Error \n");
		
		}
		dbug("Mt:Message From Queue event %d\n",event_monitor.msg_data.event);
#if 1
		/*Read From Queue*/
		
		
		switch(event_monitor.msg_data.event )
		{
			case MOTION_DETECTED:
			{
				dbug("Monitor: MOTION DETECTED EVENT\n");
				strncpy(msg_buff,"CALC_DISTANCE",MAX_MESSAGE_SIZE);
				/*Informing Event to Distance Calculate  Thread */
				if( FAIL == post_event( pipe_t2 , msg_buff, MAX_MESSAGE_SIZE ) )
				{
					printf("Mt: pipe Write Error %s\n",strerror(errno));
				}
				
#if 0
				/*Informing Event to Motion Calculate  Thread */
				if( FAIL == post_event( pipe_t1 , msg_buff, MAX_MESSAGE_SIZE ) )
				{
					printf("Mt: pipe Write Error %s\n",strerror(errno));
				}
#endif					
			break;
			}
			case VALID_DISTANCE:
			{
				dbug("Monitor:VALID DISTANCE EVENT\n");
				strncpy(msg_buff,"RING_BELL",MAX_MESSAGE_SIZE);
				/*Informing Event to Distance Calculate  Thread */
				if( FAIL == post_event( pipe_t3 , msg_buff, MAX_MESSAGE_SIZE ) )
				{
					printf("Mt: pipe Write Error %s\n",strerror(errno));
				}
					
			
			
			break;
			}
			case INVALID_DISTANCE:
			{
				dbug("Monitor:INVALID DISTANCE EVENT");
					
				strncpy(msg_buff,"MOTION_SCAN_ON",MAX_MESSAGE_SIZE);
				/*Informing Event to Distance Calculate  Thread */
				if( FAIL == post_event( pipe_t3 , msg_buff, MAX_MESSAGE_SIZE ) )
				{
					printf("Mt: pipe Write Error %s\n",strerror(errno));
				}
			
			
			break;
			}	
	
			default:
			printf("Db:Invalid Event in Monitor Thread \n");
		
		}	


#endif

	}


	pthread_exit(NULL);



return NULL;
}




/* Function Call Definition */
/*
 * Getting Input Value From File
 * read_data Object returned From Function call
 * */
			
read_data get_value(char *file_name)
{
	read_data data_val={0};

	FILE * arg_file = NULL;
 	if ( NULL == (arg_file = fopen(file_name,"r") ) )
	{
		printf("%s Fopen Error  %s\n",file_name, strerror(errno));
		exit(1);
	}	
	
	
	if(0 == fread( &data_val.value,1,1,arg_file ))
	{
		printf("fread Error %s \n",file_name);
		exit(1);
	}
	        data_val.value -=48;			// Ascii to Int Conversion

	if( 0 != fseek(arg_file,2,SEEK_SET))
	{
		printf("Fseek Error %s\n",file_name);
		exit(1);
	
	}

	if(0 == fread( &data_val.status,1,1,arg_file ))
	{
		printf("fread Error %s \n",file_name);
		exit(1);
	}
	data_val.status -=48;				//Ascii to Int Conversion

	if(0 != fclose(arg_file))
	{
		printf("fclose Failed %s error=%s\n",file_name,strerror(errno));
		exit(1);
	}

 return data_val;
}

/*
 * status_t post_event();
 *
 * */
status_t post_event(int *pipe_arg,char *msg_buff,int msg_size)
{
		if( -1 == write( pipe_arg[ PIPE_WRITE ] , msg_buff, msg_size ) )
		{
			printf("Mt: pipe Write Error %s\n",strerror(errno));
			return FAIL;
		}
		else
		{
			return SUCCESS;
		}

}

/*
 * status_t read_event(int *pipe_arg,char *msg_buff )
 * 
 *
 *
 * */

status_t read_event(int *pipe_arg, char *msg_buff, int msg_size )
{

	    if(-1 == read( pipe_arg[PIPE_READ], msg_buff , msg_size ) )
	    {
	     	printf("Dt: Pipe read Error\n");
	      return FAIL;
	    }
	    else
	    {
	    	return SUCCESS;
	    }
	
}


/*
 * Post Message to Queue
 *
 * status_t post_messageToQ( event_data );
 * 
 * Queue Posted by three threads except Monitor thread
 *
 * */
status_t post_messageToQ(event_data event_arg)
{

	     event_arg.mtype = 1;		// This Element Needed Which Help at receving End
						// If this is not present in structure send Error Will occure	     
	     dbug("msgQId =%d\n",msgQ_Id);
	    if(FAIL ==  msgsnd(msgQ_Id,&event_arg,(size_t)sizeof(msg_data),0) )
	    {

	    	printf("%d:warn: message Send Error %s\n",pthread_self(),strerror(errno));
	    return FAIL;	
	    }
	    
	return SUCCESS;
}

/*
 *
 * status_t get_messageFromQ(event_data *)
 *  Passing Argument will be address of Object
 *  Data will be Filled In object
 *
 *  status Will be returned SUCCESS or FAIL
 *
 *  Queue Will Be read By Monitor thread
 *
 *
 * */

status_t get_messageFromQ(event_data *data_arg)
{
		// Clearning Object
		memset(data_arg, 0 , sizeof(event_data) );

		if (-1 == msgrcv(msgQ_Id, data_arg, sizeof(msg_data),0,0) )	// Always read First message of Queue
		{
			printf("warn: Message Read Error %s \n",strerror(errno));
			return FAIL;
		}

	return SUCCESS;
}

/*
 * 
 *	status_t check_thread_alive(void);
 *     	Function work on linux ps command
 *     	it check thread is there or not in PS command
 *     	that way it knows threads are alive or not
 *
 *     	it may create File to store PS command
 *
 * */


status_t check_thread_alive(void)
{



}


/*
 * String to enum conversion function
 * Helpful for implementing Switch case
 * event str_enum(char * )
 *
 * */

event str_enum( char *arg )
{
	if(arg == NULL)
	{
		return INVALID;
	}

	else if( 0 == strcmp(arg,"CALC_DISTANCE") )
	{
		return CALC_DISTANCE;
	}
	else if( 0 == strcmp(arg,"MOTION_SCAN_ON") )
	{
		return MOTION_SCAN_ON;
	}
	else if( 0 == strcmp(arg,"RING_BELL") )
	{
		return RING_BELL;
	}
	else
	{
		
		return INVALID;
	}
}



/*
 * shutdown()
 *
 * Stop all Process and releases the resources
 * */

void shutdown()
{

	dbug("No shutdown implemented");

}

