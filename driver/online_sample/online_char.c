#include<linux/init.h>
#include<linux/module.h>

#include<linux/fs.h>
#include<linux/types.h>
#include<linux/kdev_t.h>
#include<linux/cdev.h>

#include<linux/mutex.h>

#include<asm/uaccess.h>


#define MHELLO_STACK_SIZE 20

static int major;
static dev_t dev;
static struct cdev my_cdev;
static int count;
static DEFINE_MUTEX(lock);
static char stack[MHELLO_STACK_SIZE];
static int head;


static ssize_t hello_read(struct file* f,char* buf,size_t count,loff_t * offset)
{
    unsigned int res;
    printk(KERN_ALERT"R\n");
/*
    mutex_lock(&lock);

    if(head<0)
    {
        mutex_unlock(&lock);
        return 0;
    }
    res=copy_to_user(buf,stack+head,1);
    head--;

    mutex_unlock(&lock);
 */
    return 1;
}
static ssize_t hello_write(struct file* f,const char* buf,size_t count,loff_t * offset)
{
    unsigned int res;
    printk(KERN_ALERT"W\n");
/*
    mutex_lock(&lock);

    if(head >= MHELLO_STACK_SIZE-1)
    {
        mutex_unlock(&lock);
        return -ENOSPC;
    }
    head++;
    res=copy_from_user(stack+head,buf,1);

    mutex_unlock(&lock);
 */
    return 1;
}

static int hello_open(struct inode* inode,struct file* f)
{
    mutex_lock(&lock);
    count++;
    printk(KERN_ALERT"O %d\n",count);
    mutex_unlock(&lock);
    return 0;
}
static int hello_release(struct inode* inode,struct file* f)
{
    mutex_lock(&lock);
    count--;
    printk(KERN_ALERT"C %d\n",count);
    mutex_unlock(&lock);
    return 0;
}
const struct file_operations fops=
{
    .owner=THIS_MODULE,
    .read=hello_read,
    .write=hello_write,
    .open=hello_open,
    .release=hello_release
};

static int hello_init(void)
{
    int result=-1;

    mutex_init(&lock);
    mutex_lock(&lock);

    count=0;
    head=-1;

    result=alloc_chrdev_region(&dev,0,1,"mhello");

    if(result<0)
        printk(KERN_ALERT"Can't allocate major.\n");

    major=MAJOR(dev);
    printk(KERN_ALERT"Hello, my major no. is %d.\n",major);


    cdev_init(&my_cdev,&fops);
    my_cdev.owner=THIS_MODULE;


    cdev_add(&my_cdev,dev,1);

    mutex_unlock(&lock);
    return 0;
}
static void hello_exit(void)
{
    mutex_lock(&lock);
    unregister_chrdev_region(dev,1);
    cdev_del(&my_cdev);
    mutex_unlock(&lock);
    printk(KERN_ALERT"Goodbye world\n");
}
module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("foo");
