
/*
 * Header Contains All the function definitions 
 * which are common among multiple C File
 *  
 *  common enum and structure Typedef are
 *  present Here
 *
 * */

#ifndef DOOR_BELL_H_
#define DOOR_BELL_H_

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

/*
 * Resource related Data
 *
 * */
	int pipe_t1[2]={0};		// Motion Thread Pipe		// For Motion thread can be Config As Non Blocking
	int pipe_t2[2]={0};		// Distance Thread Pipe
	int pipe_t3[2]={0};		// Door Bell Thread Pipe

  key_t const msgQueKey = 2000 ;  // Message queue Key
  int msgQ_Id = 0;

typedef void (*timer_expiry_handler)(union sigval);

typedef enum _ecu_status{
	
	ECU_DEFAULT    = -1,
	ECU_WORKING    =  0,
	ECU_NOTWORKING =  1

}ecu_status;

typedef enum _status_t{

	SUCCESS=0,
	FAIL   =1

}status_t;

typedef enum _doorStatus{
	
	DOOR_DEFAULT = -1,
	DOOR_OPEN    =  0,
	DOOR_CLOSED  =  1
	

}doorStatus;

typedef enum _event{

	INVALID 		= -1,
	MOTION_DETECTED 	= 0,
        CALC_DISTANCE 		= 1,
	MOTION_SCAN_ON		= 2,
	INVALID_DISTANCE	= 3,
	VALID_DISTANCE		= 4,
	RING_BELL		= 5,
	RING_BELL_EXPIRY	= 6,
	ECU_NOT_WORKING		= 7,
	DOOR_WAIT_EXPIRY	= 8,
 	DOOR_WAIT_RETRY_END	= 10	
}event;

typedef void *(*_thread_routine)(void *);

typedef struct _thread_obj{

	char threadName[16];
	pthread_t thread_id;
	_thread_routine thread_routine;

}thread_obj;

typedef struct _msg_data{

	event event;
}msg_data;

typedef struct _even_data{
	long mtype;
	msg_data msg_data;

}event_data;

typedef struct _read_data{
	
	int value;
	ecu_status status;

}read_data;


#endif
