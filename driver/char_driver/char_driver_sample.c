
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
 * Note:
 * Comments are added all over Only For Educational Purpose
 * It is not following any Kernel Coding Formats
 *
 * */

#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/types.h>
#include<linux/stat.h>
#include<linux/fs.h>
#include<linux/kdev_t.h>
#include<linux/cdev.h>

MODULE_LICENSE("Dual BSD/GPL");

/* Function Prototype */
static int char_module_init(void);
static void char_module_exit(void);
static void setup_char_module(void);

ssize_t scull_read(struct file *filp,char __user *buff,size_t count,loff_t *offp);
ssize_t scull_write(struct file *filp,const char __user *buff,size_t count,loff_t *offp);
int  scull_open(struct inode *inode,struct file *filp);
int scull_release(struct inode *inode,struct file *filp);

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

struct mydevice_data dev_data;
static dev_t dev;
static void setup_char_module(void)
{
	 int err, devno;
	 	
	if(0 != alloc_chrdev_region(&dev,(unsigned int)0,1,"scull0"))
	{
	 	printk(KERN_ALERT"Allocation Failed \n");
		char_module_exit();
	}
	printk("Major Number =%d\n",MAJOR(dev));
	printk("Minor Number =%d\n",MINOR(dev));

//	err=register_chrdev_region(dev,1,"char_device_simple");
//	if(err)
//	{
//	 	printk(KERN_ALERT"register_chrdev_region Failed \n");
//		char_module_exit();
//	}

/*
 * Registration Of Module Here
 *
 * */

	devno = MKDEV(MAJOR(dev),MINOR(dev) + 1);
	cdev_init(&dev_data.cdev,&scull_fops);
	dev_data.cdev.owner= THIS_MODULE;
	dev_data.cdev.ops  = &scull_fops;
	err = cdev_add(&dev_data.cdev,dev,1);
	
	if(err)
	{
	 	printk(KERN_ALERT"Cdev add Failed \n");
		char_module_exit();
	}

	kobject_uevent(&dev_data.cdev.kobj,KOBJ_ADD);
	printk("Registration Sucessful \n");

}
ssize_t scull_read(struct file *filp,char __user *buff,size_t count,loff_t *offp)
{

	return 0;
}
ssize_t scull_write(struct file *filp,const char __user *buff,size_t count,loff_t *offp)
{

	return 0;
}
int  scull_open(struct inode *inode,struct file *filp)
{

	return 0;
}
int scull_release(struct inode *inode,struct file *filp)
{


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
	cdev_del(&dev_data.cdev);
	unregister_chrdev_region(dev,1);
	kobject_uevent(&dev_data.cdev.kobj,KOBJ_REMOVE);
	return;
}

module_init(char_module_init);
module_exit(char_module_exit);
