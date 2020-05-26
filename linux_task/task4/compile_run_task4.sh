#!/bin/bash


if [  -e p1 ]
then
	echo clean
	rm p1
	rm p2
fi	


echo Compile code

gcc -o p1 process1.c 
if [ $? -ne 0 ]
then
	echo Process1 Compilation Failed
	exit -1
fi
gcc -o p2 process2.c 
if [ $? -ne 0 ]
then
	echo process2 Compilation Failed
	exit -1
fi

echo Compilation Done...

echo run

./p1 & 		# Running process in back ground prints will come on terminal
./p2 &
