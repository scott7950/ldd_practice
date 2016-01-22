#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/semaphore.h>
#include <linux/fs.h>
#include <linux/cdev.h>

static int Major;
dev_t dev_no;

struct device {
    char array[100];
    struct semaphore sem;
} char_arr;

/*
   Linux uses a cdev structure to keep track of all the character devices, 
   struct cdev {
      struct kobject kobj;      *** shows up in the directory tree of /sys
      struct module *owner;
      const struct file_operations *ops;
      struct list_head list;
      dev_t dev;
      unsigned int count;
   };   
*/
struct cdev *arr_cdev;

int open(struct inode *inode, struct file *filp) {
    printk(KERN_INFO "Inside open \n");
    return 0;
}

int release(struct inode *inode, struct file *filp) {
    printk(KERN_INFO "Inside close \n");
    return 0;
}

ssize_t read(struct file *f, char *b, size_t cnt, loff_t *o) {
    printk(KERN_INFO "Inside read \n");
    return 0;
}

ssize_t write(struct file *f, const char *b, size_t cnt, loff_t *o) {
    char buffer[128];
    cnt = (cnt > 64) ? 64 : cnt;
    strncpy(buffer, b, cnt);
    printk(KERN_INFO "Inside write, buffer=%s, size=%d\n", buffer, cnt);
    return cnt;
}

/* struct file_operations is huge - see include/linux/fs.h 
   purpose is to connect operations to the device numbers 
 
   fields: 
     struct module *owner - prevents module from being unloaded
            when its operations are in use
     open - open the device
     release - close the device
     read   - retrieves data from the device
     write  - sends data to the device
     llseek - change the current read/write cursor position in the file
     aio_read - asynchronous read - may not complete before the function returns
     readdir - for filesystems - to read directories
     poll - query whether read/write to some file descriptor will block
     ioctl - allow issue of device specific commands
     mmap - request a mapping of device memory to process address space
     flush - 
     fsync - another form of flush - slightly different from above
     lock  - implement file locking
     readv - read operation over many memory areas
     writev - write operation over many memory areas
     sendfile -
     sendpage -
     get_unmapped_area -
     check_flags -
     dir_notify -
*/

struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = read,
    .write = write,
    .open = open,
    .release = release
};

int init_module(void) {
    int ret;
    printk("---------------------------------------\n");

    // allocate a cdev structure
    arr_cdev = cdev_alloc();
    arr_cdev->ops = &fops;
    arr_cdev->owner = THIS_MODULE;
    printk(KERN_INFO "Inside init module\n");

    // allocate a major number dynamically
    // From ldd3:
    // for new drivers, we strongly suggest that you use dynamic allocation to obtain
    //your major device number, rather than choosing a number randomly from the ones
    //that are currently free. In other words, your drivers should almost certainly be using
    //alloc_chrdev_region rather than register_chrdev_region.
    ret = alloc_chrdev_region(&dev_no, 0, 1, "file_op_driver");
    if(ret < 0) {
        printk("Major number allocation is failed\n");
        return ret;
    }

    // extract major from dev_no
    Major = MAJOR(dev_no);

    if(MKDEV(Major, 0) != dev_no) {
        printk(KERN_INFO "Yikes - should show up in /proc/devices");
    }

    printk(KERN_INFO "init: Major %d\n", Major);
    ret = cdev_add(arr_cdev, dev_no, 1);
    if(ret < 0) {
        printk(KERN_INFO "Unable to allocate cdev");
        return ret;
    }

    sema_init(&char_arr.sem, 1);

    return 0;
}

void cleanup_module(void) {
    printk(KERN_INFO "Inside cleanup_module\n");
    cdev_del(arr_cdev);
    unregister_chrdev_region(dev_no, 1);
    unregister_chrdev(Major, "file_op_driver");
}

MODULE_LICENSE("GPL");
