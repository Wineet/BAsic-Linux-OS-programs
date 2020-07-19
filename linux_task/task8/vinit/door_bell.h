
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
	RING_BELL_TIMER_EXPIRY	= 6
		
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
