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
 * add seq_file interface as a debug Mechanism for Driver. 
 * create file under /proc directory proc will be as a parent Directory
 * 
 * In the proc File current number of bytes written in kernel buffer count will be there
 * 
 * Note:
 * Linked List is created for a single Node only to get demo for Seq file iterations.
 * Node stores No valuable Data so as this Char Driver
 * 
 * Data structre part is Bare minimum Used
 *
 * Comments are added all over Only For Educational Purpose
 * It is not following any Kernel Coding Formats
 *
 * Implementation summary: 
 *		Registered as Char Driver
 * 		Two Device registered In the below Driver (Maintaining same buffer)
 * 		For debugging printk is added
 * 		seq_file interface is added as a debug mechanism
 * 		As debug Data Information Number of bytes written in Device is stored
 * */

#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/proc_fs.h>		/* Proc fs interface 					*/
#include<linux/slab.h>			/* dynamic Memroy allocation 				*/
#include<linux/seq_file.h>		/* seq_file layer operations and interface		*/
#include<linux/list.h>			/* For linked List data structure			*/
#include<linux/version.h>		/* Linux kernel version Macro 				*/
#include<linux/types.h>
#include<linux/stat.h>
#include<linux/fs.h>
#include<linux/kdev_t.h>
#include<linux/cdev.h>
#include<linux/uaccess.h>       /* this is for copy_user vice vers*/


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

/* seq_file Interface Call backs */
static void *mydrv_seq_start (struct seq_file *s,loff_t *pos);
static void *mydrv_seq_next (struct seq_file *s,void *v,loff_t *pos);
void mydrv_seq_stop(struct seq_file *s,void *v);
static int mydrv_seq_show(struct seq_file *s,void *v);
static int mydrv_seq_open(struct inode *inode,struct file *filp);

#define MAX_BUFF_SIZE 1024
static char drv_buff[MAX_BUFF_SIZE];
static char *my_name="Not_given";


struct mydevice_data{

	struct cdev cdev;
	struct semaphore sem;	/*Mutual Exclusive semaphore*/
	/*  further pointers need to store here
	 * for data listing
	 * */
};

struct proc_debug_struct {
   /* ... */
   struct list_head list; /* Link to the next node */  //this is fixed for any type of struct to have iterator operation
   char info[23];			// Store Node Number
   char info1[20];         /* Info to pass via the procfs file (Storing Number Of bytes in drv_buffer) */
   /* ... */
};
/* List Head 
 * Linked List will be get initilised at compile time
 * Because of Below Macro
 * Linked list type is Doubly Circular linked list
 * */
static LIST_HEAD(mydrv_list_head);  
/*
 *  To Get loading Parameter for Driver
 *  S_IRUGO  Flag is used for Read Only parameter can not be edited
 *  S_IRUGO | S_IWUSR Flags used for sudo user can only edit Parameter
 *  S_IRUSR  User read Permission
 */
module_param(my_name,charp,S_IRUGO);	

struct mydevice_data dev_data;
static dev_t dev;
static struct proc_dir_entry *proc_dir_ptr 	= NULL;
static struct proc_dir_entry *proc_parent_dir	= NULL;


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
//every kernel module object file creates a struct module{} in the 
//system space, when loaded - this struct module{} is used to manage
//the loaded kernel module object file, in the system !!!

/*
 * seq_open will be invoked when ever proc file is touched for reading/writing
 * seq_read,seq_lseek, seq_release function are already provided by kernel headers
 * 
 * we only need to specify mydrv_seq_open i.e. "seq_open" because in this we will be 
 * registering another set of call backs with sequence file layer of kernel  
 */
static struct file_operations mydrv_proc_fops = {
   .owner    = THIS_MODULE,    //this macro will provide the ptr to our module object
   .open     = mydrv_seq_open, /* User supplied */  //passing addresses of functions 
                                                    //to function pointers
   .read     = seq_read,          //Built-in helper functions taken from 
                                  //fs/seq_file.c 
   .llseek   = seq_lseek,         //fs/seq_file.c provides sequence file
                                  //layer - sequence file layer provides
   .release  = seq_release,       //support to procfs layer 
};

/* whenever, our procfs file is accessed, associated data structure
 * is navigated and information extracted from objects maintained
 * in the data structure  are copied to users-space buffer - however, 
 * the whole frame-work works, if you have written these methods as per 
 * rules and validated their working !!!
  */
/* Define iterator operations */
static struct seq_operations scull_seq_ops = {
   .start  = mydrv_seq_start,
   .next   = mydrv_seq_next,
   .stop   = mydrv_seq_stop,
   .show   = mydrv_seq_show,
};
int proc_read_fun(char *buf,char **start,off_t offset,int count,int *eof,int *data)
{

	return 0;
}

static void setup_char_module(void)
{
	 int err, devno;
	 struct proc_debug_struct *proc_list_node;
	 struct proc_debug_struct *p = NULL;
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

	/* creating proc entry for seq_file interface
	 * Arg1: File Name Which will appear in file system
	 * Arg2: Permission for file acess
	 * Arg3: pointer to parent Directory if passed "NULL" then
	 * 		 Default parent will be /proc
	 * 
	 * return type in struct proc_dir_entry *  
	 * */

	/* 
	 * Below API is removed in kernel version 3.10 and above 
	 * Please Use proc_create API for creating Entry
	 *
	 * create_proc_entry	Create/adds entry in proc virtual file system 
	 * procfs is also known as psuedo file system
	 * 
	 * arg1: File name
	 * arg2: File Permissions
	 * arg3: parent Directory If passed NULL then default parent will be "/proc"
	 * */
#if LINUX_VERSION_CODE <= KERNEL_VERSION(3,10,0)
	proc_dir_ptr=create_proc_entry("seq_file",S_IRUSR,NULL);
	if(proc_dir_ptr != NULL)
	{
		/* After proc dir created under /proc 
		 * Now we need to register proc file operations
		 * registeration we need to do with pointer "proc_dir_ptr"
		 * because it is an object pointing to proc dir file system
		 * */
		proc_dir_ptr->proc_fops = &mydrv_proc_fops;

	}
	else
	{			
		printk(KERN_ALERT"Proc Entry creation Failed\n");
	}
#else
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
	proc_dir_ptr   = proc_create("seq_file",S_IRUSR,proc_parent_dir,&mydrv_proc_fops);
	if(proc_dir_ptr == NULL)
	{
		printk(KERN_ALERT"Proc Entry creation Failed\n");
	}
		proc_dir_ptr->owner=THIS_MODULE;
	//proc_dir_ptr->read_proc ;
#endif
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

/* procfs seq File operation
 * Linked List Initilisation
 */
/*
 * Kmalloc dynamically allocates the memory for our list
 * GFP_KERNEL: Allocate normal kernel ram. May sleep
 * writing info with Generic string
 * info1 will be actual data which we need at debug time
 */



	proc_list_node = kmalloc(sizeof(struct proc_debug_struct),GFP_KERNEL);
	sprintf(proc_list_node->info,"List Node %d\n",1);
	sprintf(proc_list_node->info1,"byte Count=%d\n",0);
/* list_add_tail adds the node at the end of the list
 * and initilise adding node list pointer as null
 * mydrv_list_head is head of list, initilisation of list done by macro LIST_HEAD(mydrv_list_head)
 */
	list_add_tail(&proc_list_node->list,&mydrv_list_head);

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
	printk("read1:count =%u, bytes_read=%d , bytes_to_read=%d *offp =%lld\n",count,bytes_read,bytes_to_read,*offp);
	down(&m_data->sem);		// Semaphore Lock
	bytes_read=bytes_to_read-copy_to_user(buff,drv_buff+ *offp, bytes_to_read);
	*offp+=bytes_read;
	printk("read2:count =%u, bytes_read=%d , bytes_to_read=%d *offp =%lld\n",count,bytes_read,bytes_to_read,*offp);
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
	struct proc_debug_struct *p=NULL,*q=NULL;
	/* programmer define structure obtained 
	 * filp pointer is initlised in scull_open function call
	 */
	struct mydevice_data *m_data = filp->private_data;	
	int max_bytes=MAX_BUFF_SIZE - *offp;
	if (count>max_bytes)
		bytes_to_write=max_bytes;
	else
		bytes_to_write=count;
	printk("write1:count =%lu, bytes_write=%d , bytes_to_write=%d *offp =%lld\n",count,bytes_write,bytes_to_write,*offp);
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
	printk("write2:count =%lu, bytes_write=%d , bytes_to_write=%d *offp =%lld\n",count,bytes_write,bytes_to_write,*offp);
	up(&m_data->sem);		// Semaphore UnLock


	if(bytes_write > 0 && bytes_write<MAX_BUFF_SIZE)
	{
		list_for_each_entry(p,&mydrv_list_head,list);	//p is head Now
		list_for_each_entry(q,p->list.next,list);	// Now q is next Node of structre type  where we will write Number of byte
		sprintf(q->info1,"byte Count=%d\n",bytes_write);
	}
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

/*********** Procfs Seq Call Back Defined Here ****************/

/* seq file interface open function call back */
static int mydrv_seq_open(struct inode *inode,struct file *filp)
{
	printk(KERN_INFO"mydrv_seq_open\n");
	/*
	 * We replace seq_open with mydrv_seq_open 
	 * This will be invoked when procfs file will be opened for operation
	 * here we are registering for sequential file layer, so iterator call backs will
	 * be performed in that manner
	 * */

return seq_open(filp,&scull_seq_ops);
}

//typically, it is 0, when navigation is started by a read()(first read) 
//during run-time,*pos tells us where are we in the list access - 
//specific object's index 
//             in the list -
//             the specific node in the list  

//param1 is the object created for this active file in the 
//sequence file layer - it may be used in our operations ' code 
//
//param2 is pointer to the offset field in the open file object of 
//this active file - however, in this context it does not represent 
//file logical byte no - instead, it represents index of the system 
//object , in the system's data structure that is navigated, when 
//this procfs file is accessed using an active file !!! in fact, when 
//an active file of this procfs file is accessed/navigated/read, 
//data from system objects maintained in the corresponding system 
//data structure is retrieved and passed to user-space !!!
//
//system passes a *pos (offset/index of an object), as per the
//current state of navigation/access !!! to start with the value will
//be 0 - as one or more objects are navigated/accessed, this value will
//be incremented - we must interpret this value, validate the value and
//take appropriate actions, as per rules !!!
//
//as per rules, we must do the following, in xxx_seq_start() :
//- using the *pos(index value), navigate related system data-structure
//  and extract ptr to
//  corresponding object 
//- return ptr to the extracted object, if there is such object for
//  the provided index !!!
//- otherwise return NULL 
//- all we need to code is the above rules and implement the same !!!

 static void *mydrv_seq_start (struct seq_file *s,loff_t *pos)
 {
	 struct proc_debug_struct *p =NULL;
	 loff_t off = 0;
	 printk(KERN_INFO"mydrv_seq_start");
	 /*Find Head Of linked List and send it further*/
	 /*returns structre address of mentioned Node
	  * arg1: pointer of node structure
	  * arg2: List Head 
	  * arg3: structure type
	  */
	 printk("seq_start pos=%d,off=%d\n",*pos,off);
	  list_for_each_entry(p, &mydrv_list_head, list);
	 /* In this case seq_file and Pos is not used*/
	 if(*pos == off++)
	 	return (void *)p->list.next;
	 else
		 return NULL;
 }
 /* mydrv_seq_next it's main implementation is for traversing
  * Unitl next returns NULL, " ..next->show->next->show.. "  loop will be running
  *  void *v is a pointer returned by *start or previous *next function call back
  * */
 static void *mydrv_seq_next (struct seq_file *s,void *v,loff_t *pos)
 {
	 struct list_head  *p_list_head = ((struct proc_debug_struct *)v)->list.next;

	 printk(KERN_INFO"mydrv_seq_next");
	 /* Will receive structure pointer from start or next
	  * obtaine next node and send/return structre Pointer Further
	  * If NULL returned means no more data is there to read 
	  * It will invoke stop
	  */
   /* 'v' is a pointer to the iterator returned by start() or
     by the previous invocation of next() 
   * list.next is attribute for next list head,
   * it's type "struct list_head" 
   *  by getting this next head we can obtain next node  "struct proc_debug_struct" pointer
   * and pass it to next call back *show
   */

	//example of list entry to understand function
	//struct proc_debug_struct *next_node=list_entry(p_list_head,struct proc_debug_struct,list);		 

  /* Due to Doubly Circular Linked list if come back to head
   * then to exit the iterator call back returning NULL
   * */
	 return (p_list_head != &mydrv_list_head)?(list_entry(p_list_head,struct proc_debug_struct,list)):NULL;
 }
 void mydrv_seq_stop(struct seq_file *s,void *v)
 {
	 printk(KERN_INFO"mydrv_seq_stop");
	 /* If .next or .start returns NULL pointer
	  * then stop will invoked
	  * stop should do clean up activity
	  *  */
	 return;
 }
 /* mydrv_seq_show sends data to user space
  * printk sends data to system logs only
  * use of seq* interface is preferred to transfer data to user space
  * sequential file layer object is already Generated (struct seq_file), by using that object
  * and seq interface function we can send data to user space
  *
  * */
 
 static int mydrv_seq_show(struct seq_file *s,void *v)
 {
	 struct proc_debug_struct *p = ((struct proc_debug_struct *)v);
	
	/* using seq_printf() we can pass info. to user space, via
     * sequence file layer's kernel buffer !!!first data is copied
     * into kernel buffer from the object of a subsystem or component !!

    * kernel buffer is used with seq_printf(provided by sequence file layer)
    * we are copying data from a current object of navigation into
    * kernel buffer of *s
	*/
		 seq_printf(s,"info:%s",p->info);
		 seq_printf(s,"%s",p->info1);
	 /* It will only print the data
	  * No pointer data modification
	  * return 0 means .show function call exectued properly
	  * After Show iterator will invoke *next function call
	  */
	  printk(KERN_INFO"mydrv_seq_show %s",p->info);
	 return 0;
 }



/*********** Procfs Seq Call Back End Here ****************/

static int __init char_module_init(void)
{
	printk("char_Module_init =%s\n",my_name);
	setup_char_module();
	return 0;
}

static void __exit char_module_exit(void)
{
	struct proc_debug_struct *p=NULL,*n=NULL;
	printk("char_module_exit \n");
	/* Deleting All Entries Of list*/
	list_for_each_entry_safe(p,n,&mydrv_list_head,list)
	{
		/*By this API no need to use list entry API*/
		kfree(p);
	}

	unregister_chrdev_region(dev,2);
	cdev_del(&dev_data.cdev);
//	cdev_del(&dev_data2.cdev);

	//Remove Proc Entry
	remove_proc_entry("seq_file",proc_parent_dir);
	remove_proc_entry("seq_file",NULL);

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
