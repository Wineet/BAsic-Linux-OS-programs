typedef enum timer_state{

	NOT_RUNNING,
	RUNNING

}timer_state;


static timer_t door_bell_timer_id      = 0;		// Door Bell Timer ID to ring the bell 2.5 sec
static timer_t door_open_wait_timer_id = 0;		// 30 sec Door open Wait Timer
static timer_t reset_states_timer_id   = 0;		// 10 Min timer if Door Doesn't Open after 3 times ringing


/*
 * Timer Function
 *
 * tmr_create();
 *
 * */

timer_t tmr_create(timer_expiry_handler fun_arg ,char *handler_name)
{
	timer_t timer_id = 0;
	struct sigevent segv;

	pthread_attr_t thread_attr;

	if( 0 != pthread_attr_init(&thread_attr) )
	{
		printf(" Warn: thread_attr init failed\n");
	}
	if( 0 != pthread_attr_setdetachstate(&thread_attr,PTHREAD_CREATE_DETACHED) )
	{
		printf("Warn: Thread Detached attr Failed \n");	
	}
	
	segv.sigev_notify	 	= SIGEV_THREAD;
	segv.sigev_value.sival_ptr 	= (void *) handler_name; 
	segv.sigev_notify_function 	= fun_arg ;
	segv.sigev_notify_attributes 	= &thread_attr;

	if ( 0 != timer_create(CLOCK_REALTIME,&segv,&timer_id) )
	{
		printf("Warn: timer Creation Failed");
		return 0;
	}
	else
	{
		return timer_id;
	}	


}

/*
 * timer_start()
 * All timer are started as one shot timer only
 *
 * */
status_t timer_start(timer_t timer_id,int delay_sec)
{
	struct itimerspec timer_spec;

	timer_spec.it_interval.tv_sec	= 0;		// No interval Based Timer
	timer_spec.it_interval.tv_nsec	= 0;
	timer_spec.it_value.tv_sec 	= delay_sec;		// second Parameter is nano_sec
	timer_spec.it_value.tv_nsec	= 0;


	if( 0 != timer_settime(timer_id,0,&timer_spec,NULL))
	{
		printf("warn: timer_start Failed \n");
		return FAIL;
	
	}
	else
	{
	
		return SUCCESS;
	}

}


/*
 * timer_stop()
 *
 * */

status_t timer_stop(timer_t timer_id)
{
	struct itimerspec timer_spec;
	timer_spec.it_interval.tv_sec	= 0;		// No interval Based Timer
	timer_spec.it_interval.tv_nsec	= 0;
	timer_spec.it_value.tv_sec 	= 0;		// second Parameter is nano_sec
	timer_spec.it_value.tv_nsec	= 0;

	if( 0 != timer_settime(timer_id,0,&timer_spec,NULL))
	{
		printf("warn: timer_stop Failed \n");
		return FAIL;
	
	}
	else
	{

		return SUCCESS;
	}


}

/*
 * is_timer_running();
 *
 * */
status_t is_timer_running(timer_t timer_id)
{
	



}



/*
 * destroy_timer();
 *
 * */
status_t timer_destroy(timer_t timer_id)
{
	
	if( -1 == timer_delete(timer_id) )
	{
		printf("Timer Destroy Failed \n");
		return FAIL;
	}
	else
	{
		return SUCCESS;
	}

}



