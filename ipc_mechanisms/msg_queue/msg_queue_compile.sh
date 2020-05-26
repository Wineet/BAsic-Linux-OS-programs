#!/bin/bash


if [ "$1" = "clean" ]
then
	rm q1 q2
	echo "Cleaned"
	exit 0
fi


gcc -o q1 queue_p1.c
if [ $? -ne 0 ]
then
	echo "Compile Failed"
	exit 1
fi

gcc -o q2 queue_p2.c
if [ $? -ne 0 ]
then
	echo "Compile Failed"
	exit 1
fi

