#!/bin/bash

g++ -o sema semaphore_linux.cpp -pthread

if [ $? -eq 0 ]
then
	echo Compileation successful
	echo program Running
	./sema &
else
	echo Compilation Failed
fi

echo Script End
