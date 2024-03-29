/* 
 * Author :  Vinit
 * Platform: R-pi
 * OS:	     Linux
 *
 * */


/*
 * Statement: 
 * Design a simple Char driver with open release read write operations
 * Use the drvier From user space to cross check the driver
 * 
 * Implement simple Proc interface, form where we can read Values
 * How many bytes wriiten to driver those will be stored
 *  
 * Note:
 * Comments are added all over Only For Educational Purpose
 * It is not following any Kernel Coding Formats
 *
 * Implementation summary: 
 *		Registered as Char Driver
 * 		Two Device registered In the below Driver (Maintaining same buffer)
 * 		For debugging printk is added
 * */

#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/types.h>
#include<linux/stat.h>
#include<linux/fs.h>
#include<linux/kdev_t.h>
#include<linux/cdev.h>
#include<linux/uaccess.h>       /* this is for copy_user vice vers				*/
#include<linux/proc_fs.h>		/* Proc fs interface 							*/
#include<linux/seq_file.h>		/* seq_file layer operations and interface		*/
/*General purpose licenese*/
MODULE_LICENSE("Dual BSD/GPL");

/* Function Prototype */
static int char_module_init(void);
static void char_module_exit(void);
static void setup_char_module(void);

ssize_t scull_read(struct file *filp,char __user *buff,size_t count,loff_t *offp);
ssize_t scull_write(struct file *filp,const char __user *buff,size_t count,loff_t *offp);
int  scull_open(struct inode *inode,struct file *filp);
int scull_release(struct inode *inode,struct file *filp);
int proc_read_fun(char *buf,char **start,off_t offset,int count,int *eof,int *data);

#define MAX_BUFF_SIZE 1024
static char drv_buff[MAX_BUFF_SIZE];
static char *my_name="Not_given";

/*
 * SCULL (simple character utlity for loading localities)
 * Scull Structure
 * */
struct file_operations scull_fops={
	
	.owner 	 = THIS_MODULE,
//	.llseek	 = scull_llseek,
	.read  	 = scull_read,
	.write 	 = scull_write,
	.open  	 = scull_open,
	.release = scull_release,
};



struct mydevice_data{

	struct cdev cdev;
	struct semaphore sem;	/*Mutual Exclusive semaphore*/
	/*  further pointers need to store here
	 * for data listing
	 * */
};

/*
 *  To Get loading Parameter for Driver
 *  S_IRUGO  Flag is used for Read Only parameter can not be edited
 *  S_IRUGO | S_IWUSR Flags used for sudo user can only edit Parameter
 *  
 */
module_param(my_name,charp,S_IRUGO);	
struct proc_dir_entry *proc_parent_dir = NULL;
struct proc_dir_entry *proc_dir_ptr	   = NULL;

struct mydevice_data dev_data;
static dev_t dev;
int proc_read_fun(char *buf,char **start,off_t offset,int count,int *eof,int *data)
{

	return 0;
}

static void setup_char_module(void)
{
	 int err, devno;
/* alloc_chrdev_region
 *
 * arg1: device Number varaible address
 * arg2: First Minor Number
 * arg3: count for devices
 * arg4: Name for Module (can be seen Under Direcotry "/proc/devices")
 *
 * Below Function call assign Dynamic Device Number to Your Device
 * Major and Minor Number which are free in Device tree are assigned
 * dev Variable will be holding Values
 * returns 0 in success
 * Device name and Major number appear in file "/proc/device"
 * */	 	
	if(0 != alloc_chrdev_region(&dev,(unsigned int)0,2,"scull"))
	{
	 	printk(KERN_ALERT"Allocation Failed \n");
		char_module_exit();
	}
	printk("Major Number =%d\n",MAJOR(dev));
	printk("Minor Number =%d\n",MINOR(dev));

/*
 * Below FUnction Call assign programmer Defined Device Number
 * Used to allocate static device Number
 * Free Device number can be seen in /proc/devices
 * If number is already is in Use allocation will be Fail
 *
 * */
//	err=register_chrdev_region(dev,1,"char_device_simple");
//	if(err)
//	{
//	 	printk(KERN_ALERT"register_chrdev_region Failed \n");
//		char_module_exit();
//	}

/*
 * Registration Of Module Here
 * MKDEV is a Macro Helps to Produce Device Number From MAJOR and Minor Number 
 * cdev_init initilise cdev structure associcated With Character Driver
 * Module registration with Function Call Backs are Important
 * cdev_add add structure to Device
 * */

	devno = MKDEV(MAJOR(dev),MINOR(dev) + 0);
/* cdev_init
 * arg1: cdev sturcture Address, File operation sturcture pointer
 * Kernel uses cdev structure type to maintain char devices internally
 * Below Function call initilize cdev stucture add it's File operations
 * */
	cdev_init(&dev_data.cdev,&scull_fops);
	dev_data.cdev.owner= THIS_MODULE;
	dev_data.cdev.ops  = &scull_fops;

/* cdev_add :
 * arg1: cdev structure address
 * arg2: device Number
 * arg3: count decides how many devices are handled with the module/device
 * 
 * After initilisation of character Device and it's registration at the end cdev_add
 * should be called. at this point you device will be able to handle file operations
 *
 * */
	err = cdev_add(&dev_data.cdev,dev,2);
	
	if(err)
	{
	 	printk(KERN_ALERT"Cdev add Failed \n");
		char_module_exit();
	}

	/* Semaphore initlisation Here
	 * Initial value of semaphore is 1 
	 */
	sema_init(&dev_data.sem,1);


#if 0
	devno = MKDEV(MAJOR(dev),MINOR(dev) + 1);
	cdev_init(&dev_data2.cdev,&scull_fops);
	dev_data.cdev.owner= THIS_MODULE;
	dev_data.cdev.ops  = &scull_fops;
	err = cdev_add(&dev_data2.cdev,dev,1);
	
	if(err)
	{
	 	printk(KERN_ALERT"Cdev add Failed \n");
		char_module_exit();
	}
#endif

//	kobject_uevent(&dev_data2.cdev.kobj,KOBJ_ADD);
	printk("Registration Sucessful \n");

		/*
	 * proc_mkdir "Create" directory and make entry in proc VFS (pseudo File system)
	 * arg1: Directory Name
	 * arg2: parent Directory, If NULL then parent direcotry will be "/proc" 
	 * */

	proc_parent_dir= proc_mkdir("seq_file",NULL);
	/* proc_create "Create" File and make entry in proc VFS (pseudo File system)
	 * arg1: file Name 
	 * arg2: file Permissions
	 * arg3: Parent Directory address/pointer, If NULL parent direcotry will be "NULL"
	 * arg4: registered proc file operations address (struct file_operations )
	 * */

	proc_dir_ptr   = proc_create("seq_file",S_IRUSR,proc_parent_dir,&my_proc_read);
	if(proc_dir_ptr == NULL)
	{
		printk(KERN_ALERT"Proc Entry creation Failed\n");
	}


}


/* Old Values Persist in this Module
 * Only Writing to buffer clears the Buffer
 * Detiled comments are mentioned in write call
 * */
ssize_t scull_read(struct file *filp,char __user *buff,size_t count,loff_t *offp)
{
	int bytes_read= -1;
	int bytes_to_read=0;
	struct mydevice_data *m_data = filp->private_data;	
	int max_bytes=MAX_BUFF_SIZE - *offp;
	if (count>max_bytes)
		bytes_to_read=max_bytes;
	else
		bytes_to_read=count;
	printk("read1:count =%d, bytes_read=%d , bytes_to_read=%d *offp =%d\n",count,bytes_read,bytes_to_read,*offp);
	down(&m_data->sem);		// Semaphore Lock
	bytes_read=bytes_to_read-copy_to_user(buff,drv_buff+ *offp, bytes_to_read);
	*offp+=bytes_read;
	printk("read2:count =%d, bytes_read=%d , bytes_to_read=%d *offp =%d\n",count,bytes_read,bytes_to_read,*offp);
	up(&m_data->sem);		// Semaphore Un-Lock
	return bytes_read;
}
/* scull_write
 *
 * arg1: File pointer (in this case dev_data )
 * arg2: user buffer (User level program buffer which wants to write)
 * arg3: Number of bytes count which needs to be written
 * arg4: Offset pointer for Buffer (maintains Read write offset )
 *
 * scull_ write copy data from user buffer to kernel Buffer
 *
 * */

ssize_t scull_write(struct file *filp, const char __user *buff, size_t count, loff_t *offp)
{

	int bytes_write= -1;
	int bytes_to_write=0;
	/* programmer define structure obtained 
	 * filp pointer is initlised in scull_open function call
	 */
	struct mydevice_data *m_data = filp->private_data;	
	int max_bytes=MAX_BUFF_SIZE - *offp;
	if (count>max_bytes)
		bytes_to_write=max_bytes;
	else
		bytes_to_write=count;
	printk("write1:count =%d, bytes_write=%d , bytes_to_write=%d *offp =%d\n",count,bytes_write,bytes_to_write,*offp);
/* copy_from_user
 * arg1: Driver Buffer pointer
 * arg2: User Buffer
 * arg3: number of bytes to be written
 *
 * Function helps to copy user buffer to driver buffer,
 * It work as memory copy only but it helips to avoid threat Invalid or Null buffer derefernce 
 * If segmentation fault occures at kernel end it will create kernel panic situation
 * 
 * return - On success it returns zero, If buffer is exceed then return Number of byte that can not be written
 *
 * */
	down(&m_data->sem);		// Semaphore Lock
/* Clearing Buffer over writing Older Values */
	memset(drv_buff,0,MAX_BUFF_SIZE);
	bytes_write=bytes_to_write-copy_from_user(drv_buff+ *offp,buff, bytes_to_write);
/* Offset pointer 
 *
 * this pointer modification is mandatory other wise
 * write function will be not having control.
 *
 * careless handling of offset pointer will may lead to illegal memory writing/ Segmentation Fault
 *
 * */
	*offp+=bytes_write;
	printk("write2:count =%d, bytes_write=%d , bytes_to_write=%d *offp =%d\n",count,bytes_write,bytes_to_write,*offp);
	up(&m_data->sem);		// Semaphore UnLock
	return bytes_write;
}
/*
 * inode structure is internally used by kernel
 * it contains all opened file pointers
 *
 * inode has open device information in the i_cdev field
 * which contains cdev structure which we setup earlier
 * 
 * arg1:represents an in memory inode 
          object, which is a common entity, in 
          an active file - it is more popular
          , in the context of other active files, 
          like normal file's active or special 
          device file's active file
 * arg2: represents an object, in an 
          active - this is known as "open file 
          object" of "an active file"
 *
 * */

int  scull_open(struct inode *inode,struct file *filp)
{
	struct mydevice_data *m_data;
	printk(KERN_INFO "charDev : device opened succesfully\n");
/*
 * iminor is a macro, helps to findout which device is invoked under Module
 * imajor is also a macro helpful to find out invoked module major number
 * */
	printk(KERN_INFO"Minor No:%d, Major No:%d",iminor(inode),imajor(inode));
/*
 * container_of
 * helps you to find out your device structure for kernel struct field pointer 
 * From Registered/initilised cdev structure obtained structure
 * 
 * by help of container_of macro we are able to find structure which has cdev strucure
 * which we initilised earlier 
 * */
	m_data=container_of(inode->i_cdev,struct mydevice_data,cdev);
	/* filp file pointer passed to other call backs
	 * In read write and seek call back you will be able to access
	 */
	filp->private_data=m_data;//useful for others call backs

	return 0;
}
int scull_release(struct inode *inode,struct file *filp)
{
	   printk(KERN_INFO "charDev : device has been closed\n");
	return 0;
}

static int __init char_module_init(void)
{
	printk("char_Module_init =%s\n",my_name);
	setup_char_module();
	return 0;
}

static void __exit char_module_exit(void)
{
	printk("char_module_exit \n");
	unregister_chrdev_region(dev,2);
	cdev_del(&dev_data.cdev);
//	cdev_del(&dev_data2.cdev);

	kobject_uevent(&dev_data.cdev.kobj,KOBJ_REMOVE);
	//kobject_uevent(&dev_data2.cdev.kobj,KOBJ_REMOVE);
	return;
}

module_init(char_module_init);
module_exit(char_module_exit);



/***********************************************
 *
 *   Note For Handling Offset Pointer in read write 
 *   function Calls
 *
Initially when read or write call back get invoked Offset is '0' so writing/reading happens from zero.
to change the position of offset driver use lseek registered call back this happens when user process do seek operation.
write/read function call back needs to return the number of bytes read/write. 

specifically in case of write if we are returning zero, then it means write has not been performed, 
kernel will retry again,eventually we will end up in write call back infinite loop.

It is important in case of write always return number of bytes that was written to module,
if number of bytes written are less than user buffer count then next write callback remaing are written.

In case of read we must modify offset with number of bytes that got read, read will stop retrying when you return 0
For consistent retry of reading/writing because of limitation of buffer, offset will be modifying,
If we don't manage offset properly, we might end of continues read or write call backs or 
in some cases it will create segmentation fault which may lead to kernel Panic.

once number of bytes are written/read we must modify offset with Number of bytes written/read. 

 *
 *
 *  
 *
 ******************** END OF FILE *****************************/
