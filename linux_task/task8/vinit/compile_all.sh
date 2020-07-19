#!/bin/bash
if [ /home/pi/vinit/Linux_repo/BAsic-Linux-OS-programs/linux_task/task8/vinit/door_bell ]
then
     gcc -o door_bell door_bell.c $1
 		if [ $? -ne 0 ]
		then
   	     echo compilation Failed
	  	     exit 1	
          fi
     gcc -c -pie door_bell.c $1
 		if [ $? -ne 0 ]
		then
   	     echo compilation Failed
	  	     exit 1	
          fi
     gcc -shared -o door_bell.so door_bell.o 
 		if [ $? -ne 0 ]
		then
   	     echo compilation Failed
	  	     exit 1	
          fi
fi
echo compilation sucessful
 exit 0
