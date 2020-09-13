#!/bin/bash
if [ -f "user_process.c" ]
then
     gcc -o user_process user_process.c 
 		if [ $? -ne 0 ]
		then
   	     echo .out compilation Failed
	  	     exit 1	
          fi
     gcc -c -pie user_process.c 
 		if [ $? -ne 0 ]
		then
   	     echo .0 compilation Failed
	  	     exit 1	
          fi
	  sleep 1
# if -fPIC giving error try option -shared 
     gcc -fPIC -o user_process.so user_process.o 
 		if [ $? -ne 0 ]
		then
   	     echo .so compilation Failed
	  	     exit 1	
          fi
echo compilation sucessful
exit 0
fi
echo Compilation Failed

