
obj-m := seq_file_interface_char_driver.o

default:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

clean:
	rm *.o
	rm *.ko
	rm *.so
	rm *.mod.*
	rm *.mod

