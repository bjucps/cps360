/*
 * File: klog.c
 * Description: A device driver for a write-only device that logs writes to the kernel log
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

/* Declaration of klog.c functions */
int klog_open(struct inode *inode, struct file *filp);
int klog_release(struct inode *inode, struct file *filp);
ssize_t klog_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
void klog_exit(void);
int klog_init(void);

/* Structure that declares the usual file */
/* access functions */
struct file_operations klog_fops = {
  write: klog_write,
  open: klog_open,
  release: klog_release
};

/* Declaration of the init and exit functions */
module_init(klog_init);
module_exit(klog_exit);

/* Global variables of the driver */
/* Major number */
int klog_major = 62;

int klog_init(void) {
  int result;

  /* Registering device */
  result = register_chrdev(klog_major, "klog", &klog_fops);
  if (result < 0) {
    printk(
      "<1>klog: cannot obtain major number %d\n", klog_major);
    return result;
  }

  printk("<1>Inserting klog module\n"); 
  return 0;

}

void klog_exit(void) {
  /* Freeing the major number */
  unregister_chrdev(klog_major, "klog");

  printk("<1>Removing klog module\n");

}

int klog_open(struct inode *inode, struct file *filp) {

  int result = 0;
	
  /* Success */
  return result;
}

int klog_release(struct inode *inode, struct file *filp) {
	
  /* Success */
  return 0;
}

ssize_t klog_write(struct file *filp, const char *buf, 
                    size_t count, loff_t *f_pos) { 
 
  char loc_buf[80];

  if (count >= sizeof(loc_buf))
    count = sizeof(loc_buf) - 1;
			    
  copy_from_user(loc_buf, buf, count);
  loc_buf[count] = '\0';

  printk(KERN_ALERT "%s", loc_buf);

  *f_pos+=count; 
  
  return count; 
}

