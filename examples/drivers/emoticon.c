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

/* Declaration of emoticon.c functions */
int emoticon_open(struct inode *inode, struct file *filp);
int emoticon_release(struct inode *inode, struct file *filp);
ssize_t emoticon_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
ssize_t emoticon_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
void emoticon_exit(void);
int emoticon_init(void);

/* Structure that declares the usual file */
/* access functions */
struct file_operations emoticon_fops = {
  read: emoticon_read,
  write: emoticon_write,
  open: emoticon_open,
  release: emoticon_release
};

/* Declaration of the init and exit functions */
module_init(emoticon_init);
module_exit(emoticon_exit);

/* Global variables of the driver */
/* Major number */
int emoticon_major = 61;

int emoticon_init(void) {
  int result;

  /* Registering device */
  result = register_chrdev(emoticon_major, "emoticon", &emoticon_fops);
  if (result < 0) {
    printk(
      "<1>emoticon: cannot obtain major number %d\n", emoticon_major);
    return result;
  }

  printk("<1>Inserting emoticon module now\n"); 
  return 0;

}

void emoticon_exit(void) {
  /* Freeing the major number */
  unregister_chrdev(emoticon_major, "emoticon");

  printk("<1>Removing emoticon module\n");

}

int emoticon_open(struct inode *inode, struct file *filp) {

  filp->private_data = ":)";
  printk("<1>emoticon_open: completed.\n");
  /* Success */
  return 0;
}

int emoticon_release(struct inode *inode, struct file *filp) {
  printk("<1>emoticon_release: completed.\n");
	
  /* Success */
  return 0;
}

ssize_t emoticon_read(struct file *filp, char *buf, 
                    size_t count, loff_t *f_pos) { 
 
  int datasize = strlen(filp->private_data);
  int available_to_read;
			    
  available_to_read = datasize - *f_pos;
  printk("<1>emoticon_read: count = %ld, available_to_read = %d\n", count, available_to_read);
  if (available_to_read <= 0) 
	return 0;
	  
  if (count > available_to_read) {
    count = available_to_read;
  }
  copy_to_user(buf,filp->private_data + *f_pos,count);

  *f_pos+=count; 
  return count; 
}

ssize_t emoticon_write(struct file *filp, const char *buf, 
                    size_t count, loff_t *f_pos) { 
 
  char data[80];
			    
  printk("<1>emoticon_write: count = %ld\n", count);
  			    
  if (count > 80) 
	count = 80;
	  
  copy_from_user(data,buf,count);

  *f_pos=0;
  if (strncmp(data, "smile", count) == 0) {
     filp->private_data = ":)";
  } else if (strncmp(data, "frown", count) == 0) {
    filp->private_data = ":(";
  } else {
    filp->private_data = ":O";
  }
  return count; 
}
