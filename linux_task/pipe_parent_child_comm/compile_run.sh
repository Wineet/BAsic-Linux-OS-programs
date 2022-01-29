#!/bin/bash

rm  m

gcc -o m pipe_task.c
if [ $? -ne 0 ]
then
	echo Compilation Failed process1
	exit 1
fi
echo "Complation Sucessful"
./m &

