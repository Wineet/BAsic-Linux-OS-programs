#!/bin/bash
if [ /home/pi/vinit/Linux_repo/BAsic-Linux-OS-programs/driver/char_driver/user_process ]
then
     gcc -o user_process user_process.c 
 		if [ $? -ne 0 ]
		then
   	     echo compilation Failed
	  	     exit 1	
          fi
     gcc -c -pie user_process.c 
 		if [ $? -ne 0 ]
		then
   	     echo compilation Failed
	  	     exit 1	
          fi
     gcc -shared -o user_process.so user_process.o 
 		if [ $? -ne 0 ]
		then
   	     echo compilation Failed
	  	     exit 1	
          fi
fi
echo compilation sucessful
 exit 0
