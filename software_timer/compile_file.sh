#!/bin/bash

function clean(){

if [ $(ls -l *.out | wc -l) -ne 0  ]
then 
	rm *.out
	echo "All files are cleaned"
fi

}


clean
for file in $@
do

gcc -o "$file.out" $file -pthread -lrt

if [ $? -ne 0 ]
then
	echo "Compilation of file $1 Failed"
	exit 1
fi

done

echo "Compilation Done"
