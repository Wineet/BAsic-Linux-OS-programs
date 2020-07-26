



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
	}else if( 0 == strcmp(arg,"RING_BELL_EXPIRY") )
	{
		return RING_BELL_EXPIRY;
	}else if( 0 == strcmp(arg,"DOOR_WAIT_EXPIRY") )
	{
		return DOOR_WAIT_EXPIRY;
	}else if( 0 == strcmp(arg,"DOOR_WAIT_RETRY_END") )
	{
		return 	DOOR_WAIT_RETRY_END;
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

