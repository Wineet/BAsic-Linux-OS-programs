#!/bin/bash
if [ /home/pi/vinit/Linux_repo/BAsic-Linux-OS-programs/process_rebooting/process_reboot ]
then
     gcc -o process_reboot process_reboot.c -pthread
 		if [ $? -ne 0 ]
		then
   	     echo compilation Failed
	  	     exit 1	
          fi
     gcc -c -pie process_reboot.c -pthread
 		if [ $? -ne 0 ]
		then
   	     echo compilation Failed
	  	     exit 1	
          fi
     gcc -shared -o process_reboot.so process_reboot.o 
 		if [ $? -ne 0 ]
		then
   	     echo compilation Failed
	  	     exit 1	
          fi
fi
echo compilation sucessful
 exit 0
