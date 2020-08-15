/*
 * Current Process compiled
 * Execute File Name is dummy.out
 *
 * */


int main()
{
	while(1)
	{
		/* 
	 	 * Conitnues Running Code
		 *
		 */
		if(/* Reboot Condition*/)
		{
			process_reboot();
		}
	}

}

void process_reboot()
{
	/* release all the resources
	 * Cancel Running thread
	 * Free dynamic Memory
	 */
	if( -1 == execl("dummy.out",NULL))
	{
		printf("Reboot Failed\n");
		exit(1);
	}
return;
}

