/*
 * File: devdriver.c
 * Description: A "do-nothing" write-only device driver 
 *
 */

/* Necessary includes for device drivers */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/fs.h> /* everything... */
#include <linux/errno.h> /* error codes */
#include <linux/types.h> /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h> /* O_ACCMODE */
#include <linux/uaccess.h> /* copy_from/to_user */

MODULE_LICENSE("Dual BSD/GPL");

/* Declaration of devdriver.c functions */
int devdriver_open(struct inode *inode, struct file *filp);
int devdriver_release(struct inode *inode, struct file *filp);
ssize_t devdriver_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
void devdriver_exit(void);
int devdriver_init(void);

/* Structure that declares the usual file */
/* access functions */
struct file_operations devdriver_fops = {
  write: devdriver_write,
  open: devdriver_open,
  release: devdriver_release
};

/* Declaration of the init and exit functions */
module_init(devdriver_init);
module_exit(devdriver_exit);

/* Global variables of the driver */
/* Major number */
int devdriver_major = 63;

int devdriver_init(void) {
  int result;

  /* Registering device */
  result = register_chrdev(devdriver_major, "devdriver", &devdriver_fops);
  if (result < 0) {
    printk(
      "<1>devdriver: cannot obtain major number %d\n", devdriver_major);
    return result;
  }

  printk("<1>Inserting devdriver module\n"); 
  return 0;

}

void devdriver_exit(void) {
  /* Freeing the major number */
  unregister_chrdev(devdriver_major, "devdriver");

  printk("<1>Removing devdriver module\n");

}

int devdriver_open(struct inode *inode, struct file *filp) {

  int result = 0;

  printk(KERN_ALERT "devdriver: open requested");
	
  /* Success */
  return result;
}

int devdriver_release(struct inode *inode, struct file *filp) {
	
  printk(KERN_ALERT "devdriver: released");

  /* Success */
  return 0;
}

ssize_t devdriver_write(struct file *filp, const char *buf, 
                    size_t count, loff_t *f_pos) { 
 
  printk(KERN_ALERT "devdriver: requested to write %d bytes starting at position %d",
     (int)count, (int)*f_pos);

  *f_pos+=count; 
  return count; 
}

