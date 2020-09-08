
obj-m := char_driver_sample.o

default:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

clean:
	rm *.o
	rm *.ko
	rm *.so
	rm *.mod.*
	rm *.mod

