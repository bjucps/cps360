/*
 * File: jiffies.c
 * Description: A device driver for a read-only device that returns the number of jiffies since system start
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

/* Declaration of jiffies.c functions */
int jiffies_open(struct inode *inode, struct file *filp);
int jiffies_release(struct inode *inode, struct file *filp);
ssize_t jiffies_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
ssize_t jiffies_write(struct file *filp, char *buf, size_t count, loff_t *f_pos);
void jiffies_exit(void);
int jiffies_init(void);

/* Structure that declares the usual file */
/* access functions */
struct file_operations jiffies_fops = {
  read: jiffies_read,
  open: jiffies_open,
  release: jiffies_release
};

/* Declaration of the init and exit functions */
module_init(jiffies_init);
module_exit(jiffies_exit);

/* Global variables of the driver */
/* Major number */
int jiffies_major = 60;

int jiffies_init(void) {
  int result;

  /* Registering device */
  result = register_chrdev(jiffies_major, "jiffies", &jiffies_fops);
  if (result < 0) {
    printk(
      "<1>jiffies: cannot obtain major number %d\n", jiffies_major);
    return result;
  }

  printk("<1>Inserting jiffies module\n"); 
  return 0;

}

void jiffies_exit(void) {
  /* Freeing the major number */
  unregister_chrdev(jiffies_major, "jiffies");

  printk("<1>Removing jiffies module\n");

}

int jiffies_open(struct inode *inode, struct file *filp) {

  int result = 0;
	
  /* Allocating memory for the buffer */
  filp->private_data = kmalloc(20, GFP_KERNEL); 
  if (!filp->private_data) { 
    result = -ENOMEM;
  } else {
    // populate buffer with string containing jiffies value
    snprintf(filp->private_data, 20, "%lu\n", jiffies); 
  }

  /* Success */
  return result;
}

int jiffies_release(struct inode *inode, struct file *filp) {
  kfree(filp->private_data); // free buffer
	
  /* Success */
  return 0;
}

ssize_t jiffies_read(struct file *filp, char *buf, 
                    size_t count, loff_t *f_pos) { 
 
  int datasize = strlen(filp->private_data);
  int available_to_read;
			    
  available_to_read = datasize - *f_pos;
  if (available_to_read <= 0) 
	return 0;
	  
  if (count > available_to_read) {
    count = available_to_read;
  }
  copy_to_user(buf,filp->private_data + *f_pos,count);

  *f_pos+=count; 
  return count; 
}

