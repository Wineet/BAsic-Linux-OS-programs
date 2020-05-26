#!/bin/bash


if [ "$1" = "clean" ]
then
	rm c s
	echo "Cleaned"
	exit 0
fi


echo "****compile Client on Client Machine*******"

#gcc -o c socket_client.c
#if [ $? -ne 0 ]
#then
#	echo "Compile Failed"
#	exit 1
#fi

gcc -o s socket_server.c
if [ $? -ne 0 ]
then
	echo "Compile Failed"
	exit 1
fi

echo "************* Server Compiled *************"
