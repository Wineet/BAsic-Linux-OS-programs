#!/bin/bash

ipcrm -a   		 # Removing all IPC keys

gcc -o s1 sema_proc1.c
if [ $? -eq 0 ]
then
	gcc -o s2 sema_proc2.c
	if [ $? -eq 0 ]
	then
	./s1 &
	./s2 &
	fi
fi

echo Script End

