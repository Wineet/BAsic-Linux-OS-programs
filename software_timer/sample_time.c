/* 
 * Author : Vinit
 * Platform : eLinux
 * hardware : R-pi 3B+
 * Task : Create a software timer and register a call back to it
 */

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>


void main()
{
	time_t my_time=0;
	struct tm *tm_struct;
	while(1)
	{
		my_time = time(NULL);
		printf("My time %ld\n",my_time);
		tm_struct = localtime(&my_time);				// Depending On time Zone We need to select This function

		printf("Date:%ld/%ld/%ld\n ",tm_struct->tm_mday,tm_struct->tm_mon+1,tm_struct->tm_year+1900);
		printf("Time> %ld:%ld:%ld\n\n ",tm_struct->tm_hour,tm_struct->tm_min,tm_struct->tm_sec);
		sleep(1);
	}




return ;
}
