
/***********************************************************

 Linux Kernel Module Development Document
	Document Contains Below Listed Things
		*Noted all The Problems Faced While Developing Different Modules
		*Steps To write Different Modules and There Purposes
		*Example Logs and Code explantion





***************************************************************/

There are three Different types Of Driver, Drivers or kernel Modules are classified on the Basis of data transfer & there Work Nature
	1: Char Device Driver - Transfers and Read Backs the data by byte by byte. Bytes streams Flow in and out to Module
	2: Block Device Driver - Chunk Of memory or number of bytes transfer at onces. We can also say data transfer happend in Packets Format
	3: Network Device Driver - All Network and Security related activities are handled by this drivers/Modules

***********  Char Device Driver ****************

In few references Char device driver are also refered as SCULL(<Full Form Of Scull>).

-->> Basic Char Driver Writing Steps are as Follow Below.

	step1: >> Write a Basic Driver Function Call

		*Generate a .c /.cpp file File Needs all related Linux Header Files
		*At First Module must have __init & __exit function call. 
		  which are entry & exit points for you module when it will get loaded and unloaded.
		*Need to initlise Few More optional macros for Modules Like Module author & licenses, 
		  so kenrel will load it without any Error.  
	
		*Module License Can Be Specified By Using Below Macro. The License Mentioned in Below Macro tells it's an opensource and General Purpose Driver
		         >> MODULE_LICENSE("Dual BSD/GPL"); <<

		*Standard Format of writing init and exit function call is as below
			>>
			static int __init char_module_init(void)
			{
				/*
				 *  Module Initilisation Happens Here
				 *  If Successful then return 0;
				 *  returning Non zero value or -ve Value, tells Driver has been Not loaded
				 */			
				return 0;
			}
			<<
			>>
			static void __exit char_module_exit(void)
			{
				/* De-initlisation Happens Here
				 *  The Function Is exit point For a Driver
				 *  Function will get called at the time of Unloading Driver
				 */
				return;
			}
			<<

		*Below Macro Helps Kernel to understand Which fuction call is entry and which one is exit point.
			__init and __exit are just to notify programmer to make things more readable. 
			kernel decides entry and exit Points with the help of below Macros Only.

			>> module_init(char_module_init); <<
			>> module_exit(char_module_exit); <<

	Step2: >> Compile The Module/Driver

		*Once step1 is taken care and written into .c/.cpp File Now we are ready to compile the Driver.
		  To Compile Driver we need kernel Make file.
		
		*Kernel Modules are always compiled by using kernel headers and make file.
		  Locate kernel Make File at Location "/lib/modules/(Kernel Version)/build"
		  the File which you are going to compile, you need to inform Make file by setting variable obj-m: in make File
			like: "obj-m := char_driver_sample.o " 			
		   or You can change the Name for Generated Kernel Module by setting Below Variable in Make File
			>>> Variable Name Needed <<<
		  After Writing File Name in make File we need to write Below Commadn in our make file
			>> make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules <<
			This Command Invokes Make file which is there in kernel Header taking Your local kernel Module for build.
		*Once make File is set. you can Run "make" Command. Which will generate .ko extension File,
		   which is our loadable kernel Module.  

	Step3: >> Loading and Unloading Driver
		
		* We can check basic driver functionality is by loading and unloading it.
			 Load Driver by   >> insmod <Driver Name> <Input Parameter if any>
			 Unload Driver by >> rmmod <Driver Name>

		To check Drive Got loaded Correct or Not you can run Below Command
		" lsmod " 	Which print out all loaded kernel Modules. or check the File "/proc/devices"
		
		you can check kernel logs, if you added any by using function call "printk". It will appear in "/var/log/syslog" file	
		 Or You can check it by command "dmesg".

		#Logs After Loading Module
			Aug 30 09:53:02 raspberrypi kernel: [ 2449.094922] char_driver_sample: loading out-of-tree module taints kernel.

	Until Now If everthing is well we can proceed with step 4 Driver Registration.

	Step4: >> Registration Of Driver (Step concern about char Driver registration)
		* Driver registration is an Important part of kernel Module Programming For Complete registration we need to follow below steps 
		* Get Device Number First, Obtain free major and minor number which called as Device number too.
			Device number can be assigned dynamically or statically/coded by looking for unused device number in file /proc/devices
		* Need to initlize char driver structure module and register char driver function call backs.
		* Function call back are need to be registered first in "struct file_operations" structure.	
		* at the time of char device structure  initlisation we can mention how many minor devices need to be registered.
	
		( Cdev is the structure which needs to be initilised and added to Kernel by cdev_init and cdev_add function calls)

	Step5: >> Define Read write open close call backs

			*registered function call backs in struct file_operations need to be defined.
			when any read/write operations are performed on module File. sequentially " open >> read/write >> close " 
			call backs are invoked.
		
			* Few important things need to keep in mind when writing read/write call backs for a sample module
				1. File pointer offset need to maintain properly
				2. correct values should be return from read and write call backs
				3. Never dereference or copy user buffer directly may lead to segmentation fault
				4. always use kernel function call to copy buffer (copy_from_user & copy_to_user)
				5. Keep track of Kernel buffer Overflow
				6. If read /write function is accessed by many process use semaphore locking mechanism

	Step6: >> Driver Interfacing File/Node creation

			* mknod creates special char or block device file and attach to the device number,
				by this file driver module will interact
			sudo mknod -m 0777 <File Name> c 240 0
				240 is device major number 0 is minor Number
	Step7: >> Basic Test the Driver

			*once Done with read/write callbacks we can compile module now and check
			* open One terminal where we can monitor logs by firing the command " tail -f /var/log/syslog "
				this will help to get updated log
			* We can write and read to driver node file which we created by Firing mknod command
				These are special char device file
			*write and read operation can be performed by "echo" and "cat" command

SAMPLE DRIVER LOG:

Aug 30 09:53:02 raspberrypi kernel: [ 2449.097785] char_Module_init =Not_given
Aug 30 09:53:02 raspberrypi kernel: [ 2449.097810] Major Number =240
Aug 30 09:53:02 raspberrypi kernel: [ 2449.097816] Minor Number =0
Aug 30 09:53:02 raspberrypi kernel: [ 2449.097831] Registration Sucessful 
Aug 30 09:54:31 raspberrypi kernel: [ 2537.922409] charDev : device opened succesfully
Aug 30 09:54:31 raspberrypi kernel: [ 2537.922478] write1:count =3, bytes_write=-1 , bytes_to_write=3 *offp =0
Aug 30 09:54:31 raspberrypi kernel: [ 2537.922487] write2:count =3, bytes_write=3 , bytes_to_write=3 *offp =3
Aug 30 09:54:31 raspberrypi kernel: [ 2537.922507] charDev : device has been closed
Aug 30 09:55:12 raspberrypi kernel: [ 2579.067443] charDev : device opened succesfully
Aug 30 09:55:12 raspberrypi kernel: [ 2579.067508] read1:count =131072, bytes_read=-1 , bytes_to_read=1024 *offp =0
Aug 30 09:55:12 raspberrypi kernel: [ 2579.067529] read2:count =131072, bytes_read=1024 , bytes_to_read=1024 *offp =1024
Aug 30 09:55:12 raspberrypi kernel: [ 2579.067606] read1:count =131072, bytes_read=-1 , bytes_to_read=0 *offp =1024
Aug 30 09:55:12 raspberrypi kernel: [ 2579.067615] read2:count =131072, bytes_read=0 , bytes_to_read=0 *offp =1024
Aug 30 09:55:12 raspberrypi kernel: [ 2579.067664] charDev : device has been closed


sample Driver Program is available at >>  " https://github.com/Wineet/BAsic-Linux-OS-programs/tree/release_linux_code/driver/char_driver "


