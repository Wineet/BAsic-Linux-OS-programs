#!/bin/bash


if [  -e m ]
then
	echo clean
	rm m
fi	


echo Compile code

gcc -o m main.c -pthread
if [ $? -ne 0 ]
then
	echo Compilation Failed
	exit -1
fi

echo Compilation Done...

echo run

./m & 		# Running process in back ground prints will come on terminal
