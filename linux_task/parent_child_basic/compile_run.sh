#!/bin/bash

rm t1 t2 m

gcc -o t1 task1.c
if [ $? -ne 0 ]
then
	echo Compilation Failed process1
	exit 1
fi

gcc -o t2 task2.c
if [ $? -ne 0 ]
then
	echo Compilation Failed process2
	exit 1	
fi

gcc -o m main.c
if [ $? -ne 0 ]
then
	echo Compilation failed Main
	exit 1
fi


echo "Complation Sucessful"

./m &

