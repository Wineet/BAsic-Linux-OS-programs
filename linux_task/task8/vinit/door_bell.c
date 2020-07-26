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
 * 	                                      | | Thread 1 and thread 2 and Thread 3 receive Message From Monitor by Pipe IPC channel
 * 	                                      | |
 * Thread2 >>						  		( Not Used Queue Because No Continues Request can Occure )
 *		Distance calculator Thread 
 *		Once Trigger Received Distance will be read From Device/File
 * 		If Any thing Goes Wrong Will Update Status Variable
 *
 *					      | |
 *					      | |  Thread 1 and thread 2 and Thread 3 receive Message From Monitor by Pipe IPC channel 
 *					      |	|	  
 * Thread3 >>						  
 *		Door Bell Thread			  
 *		holds 3 sec timer forDoor Bell ring then update the status
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
#include <signal.h>
#include <time.h>
#include <limits.h>
#include <sys/ipc.h>
#include <sys/msg.h>

/* project Specific Headers*/

#include "door_bell.h"
#include "door_bell_func.c"
#include "door_timer.h"

#define MIN_DISTANCE 4
#define MAX_DISTANCE 8

/*Global Variable Here*/

static int g_monitorThread_running  =  FALSE;
static int g_motionThread_running   =  FALSE;
static int g_distanceThread_running =  FALSE;
static int g_doorbellThread_running =  FALSE;

static timer_t door_bell_timer_id      = 0;		// Door Bell Timer ID to ring the bell 2.5 sec
static timer_t door_open_wait_timer_id = 0;		// 30 sec Door open Wait Timer
static timer_t reset_states_timer_id   = 0;		// 10 Min timer if Door Doesn't Open after 3 times ringing

static timer_state door_bell_flag   = NOT_RUNNING;
static timer_state door_open_flag   = NOT_RUNNING;
static timer_state reset_timer_flag = NOT_RUNNING;

#if 0
static timer_t dummy_timer_id = 0;
void dummy_expiry_handler(union sigval arg)
{

	printf("Dummy Timer EXPIRED \n");
	if( SUCCESS != timer_stop(dummy_timer_id) )
	{
		printf("Timer stop Failed\n");
	
	}
	if(SUCCESS !=  timer_destroy(dummy_timer_id))
	{
		printf("TIMER Destroy Failed\n");
	
	}
	
}
#endif
void door_bell_expiry(union sigval arg);
void door_open_wait_expiry(union sigval arg);
void reset_states_timer_expiry(union sigval arg);
static int door_wait_expiry_count =0;

void door_bell_expiry(union sigval arg)
{
	printf("Door Bell Timer Expiry\n");
        door_bell_flag = NOT_RUNNING;
	if( SUCCESS != timer_stop(door_bell_timer_id) )
	{
		printf("Timer stop Failed\n");	
	}
				char msg_buff[ MAX_MESSAGE_SIZE ] = {0};
				strncpy(msg_buff,"RING_BELL_EXPIRY",MAX_MESSAGE_SIZE);
				/*Informing Event to Distance Calculate  Thread */
				if( FAIL == post_event( pipe_t3 , msg_buff, MAX_MESSAGE_SIZE ) )
				{
					printf("Mt: pipe Write Error %s\n",strerror(errno));
				}
				else
				{
					dbug("Timer Expiry Posted Event\n");
				}
		

}

void door_open_wait_expiry(union sigval arg)
{
	printf("Door open wait Timer Expiry\n");
	event_data event_door_wait_expiry={0};	
	door_open_flag = NOT_RUNNING;	
 	door_wait_expiry_count++;

	if( SUCCESS != timer_stop(door_open_wait_timer_id) )
	{
		printf("Timer stop Failed\n");	
	}
	if( door_wait_expiry_count >= 3 )
	{
		printf("Maz retires Exceeded\n");
		/* Start 10 min timer and End the sequece
		 * And Restart the process
		 *  Timer Will be started By Monitor thread
		 * Let Monitor thread Know 
		 * */

			event_door_wait_expiry.msg_data.event = DOOR_WAIT_RETRY_END;
			if(FAIL == post_messageToQ(event_door_wait_expiry))
	     		{
	    			printf("Md: Post Message to Q Failed");
	     	 	 }
	
		return;
	}
			event_door_wait_expiry.msg_data.event = DOOR_WAIT_EXPIRY;
			if(FAIL == post_messageToQ(event_door_wait_expiry))
	     		{
	    			printf("Md: Post Message to Q Failed");
	     	 	 }
				

}
void reset_states_timer_expiry(union sigval arg)
{
	printf("reset states Timer Expiry\n");
	char msg_buff[20]={0};
	
	reset_timer_flag = NOT_RUNNING;
	if( SUCCESS != timer_stop(reset_states_timer_id) )
	{
		printf("Timer stop Failed\n");	
	}

	/* Need to reset the states
	 * And Move Back to Motion Command
	 *
	 * */

	strncpy(msg_buff,"MOTION_SCAN_ON",MAX_MESSAGE_SIZE);
	/*Informing Event to Distance Calculate  Thread */
	if( FAIL == post_event( pipe_t1 , msg_buff, MAX_MESSAGE_SIZE ) )
	{
		printf("Mt: pipe Write Error %s\n",strerror(errno));
	}

}


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
 
 g_monitorThread_running  = FALSE;
 g_motionThread_running   = FALSE;
 g_distanceThread_running = FALSE;
 g_doorbellThread_running = FALSE;
#endif

 g_distanceThread_running = TRUE;
 g_monitorThread_running  = TRUE;
 g_motionThread_running   = TRUE;
 g_doorbellThread_running = TRUE;

#if 0 
dummy_timer_id = tmr_create(dummy_expiry_handler ,"dummy");

if( SUCCESS != timer_start( dummy_timer_id, 5) )
{
	printf("Dummy Timer start Failed \n");
}
#endif
  door_bell_timer_id 	  = tmr_create (door_bell_expiry , "Ring_bell");
  door_open_wait_timer_id = tmr_create (door_open_wait_expiry, "Door_Wait");
  reset_states_timer_id   = tmr_create (reset_states_timer_expiry, "reset_states");

if( 0 == door_bell_timer_id || 0 == door_open_wait_timer_id || 0 == reset_states_timer_id  )
{
	printf("Warn: TIMER Creation Failed\n");
}

 pthread_exit(0);
return 0;
}

/*
 * Motion Detector Thread
 *
 * */


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



/*
 * Distance Calculator Thread
 *
 * */


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
					read_data distance_data = get_value("file/distanceSensor.txt");	
	 				dbug("distance value = %d distance status = %d\n",distance_data.value, distance_data.status);	
					
					if(distance_data.status == ECU_WORKING )
					{
						 if( distance_data.value >= MIN_DISTANCE && distance_data.value <= MAX_DISTANCE )
						{
     	     						event_dist.msg_data.event = VALID_DISTANCE;
						}
						else
						{
     	     						event_dist.msg_data.event = INVALID_DISTANCE;		
						}	
					}
					else
					{
						event_dist.msg_data.event = ECU_NOT_WORKING;
						
					}
	     			
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


/*
 * Door Bell Thread
 *
 * */
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
	event_data event_bell={0};

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
				 * Start 3 Second Timer For Door Bell Ring
				 * It will post Event Back to Same thread 
				 * */
			
				if( SUCCESS != timer_start( door_bell_timer_id, 3) )
				{
					printf("Dummy Timer start Failed \n");
				}

				/*TIMER Expiry will Post Back to queue*/
			break;	
			}
			
			case RING_BELL_EXPIRY:
			{
				dbug("Door Bell: RING_BELL_TIMER_EXPIRY \n");
				/*Post Back value to queue*/	
		     		
				event_bell.msg_data.event = RING_BELL_EXPIRY;
	     			
				if(FAIL == post_messageToQ(event_bell))
	     			{
	     				printf("Db: Post Message to Q Failed");
	     			}
			

			break;	
			}
			default:
			printf("Db:Invalid Event in Door Bell \n");
		
		}
	




	}
	pthread_exit(NULL);


return NULL;
}

/*
 * Monitor Thread
 *
 *
 * */

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
				dbug("Monitor:INVALID DISTANCE EVENT\n");
					
				strncpy(msg_buff,"MOTION_SCAN_ON",MAX_MESSAGE_SIZE);
				/*Informing Event to Distance Calculate  Thread */
				if( FAIL == post_event( pipe_t1 , msg_buff, MAX_MESSAGE_SIZE ) )
				{
					printf("Mt: pipe Write Error %s\n",strerror(errno));
				}
			
			
			break;
			}	
			case RING_BELL_EXPIRY:
			{
				dbug("Monitor:RING_BELL_EXPIRY  EVENT\n");

				/* Read Door Status Here*/

					read_data door_data = get_value("file/doorStatus.txt");	
	 				dbug("door value = %d door status = %d\n",door_data.value, door_data.status);	
				if(door_data.value == DOOR_OPEN)
				{
					dbug("Door is OPENED\n");
					/* Sleep Poll Until Door Get Closed */
					
					/*
					 * Wait And Poll the Door Status To closed
					 *  Once Door is Closed again Start 
					 *  Motion Scan
					 * */
					do
					{
						 door_data = get_value("file/doorStatus.txt");	
	 					 dbug("door value = %d door status = %d\n",door_data.value, door_data.status);			
						 sleep(1);
					}
					while(door_data.value == DOOR_OPEN);

					strncpy(msg_buff,"MOTION_SCAN_ON",MAX_MESSAGE_SIZE);
					/*Informing Event to Distance Calculate  Thread */
					if( FAIL == post_event( pipe_t1 , msg_buff, MAX_MESSAGE_SIZE ) )
					{
						printf("Mt: pipe Write Error %s\n",strerror(errno));
					}
						

				}
				else if(door_data.value == DOOR_CLOSED )
				{
					/* door is still  Closed Start the timer
					 * start door wait timer 30 Sec
					 */
					dbug("Door IS CLOSED \n");
 
					if( SUCCESS != timer_start( door_open_wait_timer_id, 30) )
					{
						printf("Door open wait Timer start Failed \n");
					}	


				}

			break;
			}
			case ECU_NOT_WORKING:
			{
				dbug("Monitor: ECU_NOT_WORKING\n");
				/*Need to reboot If Any ECU is Not Working*/	
				
				/*
				 * Process Should Release Resource
				 * And Reboot itself
				 *
				 * */

			break;
			}
			case DOOR_WAIT_EXPIRY:
			{
				dbug("Monitor: DOOR_WAIT_EXPIRY\n");

				strncpy(msg_buff,"RING_BELL",MAX_MESSAGE_SIZE);
				/*Informing Event to Distance Calculate  Thread */
				if( FAIL == post_event( pipe_t3 , msg_buff, MAX_MESSAGE_SIZE ) )
				{
					printf("Mt: pipe Write Error %s\n",strerror(errno));
				}
				
#if 0				
				  if( SUCCESS != timer_start( door_open_wait_timer_id, 30) )
					{
						printf("Door open wait Timer start Failed \n");
					}
#endif
		
			break;
			}
			case DOOR_WAIT_RETRY_END:
			{
				dbug("Monitor: DOOR_WAIT_RETRY_END\n");
			
				/*  Start 10 MIN Timer Here
				 *
				 * */


				if( SUCCESS != timer_start( reset_states_timer_id, 10*60 ) )
				{
					printf("Reset Timer start Failed \n");
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



