#!/bin/bash

gcc -o door_bell door_bell.c -pthread -lrt -fPIC
if [ $? -ne 0 ]
then
	echo compilation Failed 1
		exit 1	
fi
gcc -c -pie door_bell.c -pthread -lrt -fPIC
if [ $? -ne 0 ]
then
	echo compilation Failed 2
		exit 1	
fi
gcc -shared -o door_bell.so door_bell.o -fPIC
if [ $? -ne 0 ]
then
	echo compilation Failed 3
		exit 1	
fi
echo compilation sucessful
exit 0
