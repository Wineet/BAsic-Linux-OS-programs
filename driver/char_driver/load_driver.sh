#/bin/bash

if [ "$(id -u)" != "0" ]
then
	echo "Run script with \"sudo\" "
	exit 1
fi
	
if [ $# -ne 1 ]
then
	echo "Invalid Arguments"
	echo "Arg1: load/remove"
	exit 1
fi

if [ "$1" = "load" ]
then
	/sbin/insmod char_driver_sample.ko my_name=driver_name
	MAJOR=$(awk '/scull/ { if($2=="scull")print $1;}' /proc/devices)
	echo "load $MAJOR"
	if [ -z "$MAJOR" ]
	then
		echo "No device Loaded"
		exit 1
	fi
	mknod /dev/scull0 -m 0777 c $MAJOR 0
	mknod /dev/scull1 -m 0777 c $MAJOR 1
fi

if [ "$1" = "remove" ]
then
	echo "unloading"
	rm /dev/scull0 
	rm /dev/scull1
	rmmod char_driver_sample
fi

echo Script Ended
