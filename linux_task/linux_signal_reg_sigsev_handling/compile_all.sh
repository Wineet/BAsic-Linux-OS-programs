#!/bin/bash
if [ /home/pi/vinit/Linux_repo/BAsic-Linux-OS-programs/linux_task/task7/vinit/signal_reg.c ]
then
     gcc -o signal_reg signal_reg.c -fPIC 
 		if [ 0 -ne 0 ]
		then
   	     echo compilation Failed 1
	  	     exit 1	
          fi
     gcc -c -pie signal_reg.c -fPIC
 		if [ 0 -ne 0 ]
		then
   	     echo compilation Failed 2
	  	     exit 1	
          fi
     gcc -shared -o signal_reg.so signal_reg.o -fPIC
 		if [ 0 -ne 0 ]
		then
   	     echo compilation Failed 3
	  	     exit 1	
          fi
fi
echo compilation sucessful
 exit 0
