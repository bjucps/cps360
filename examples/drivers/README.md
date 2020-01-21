# README

These examples include updated versions of the code accompanying the
article [Writing device drivers in Linux: A brief tutorial](http://freesoftwaremagazine.com/articles/drivers_linux/) by
Xavier Calbet, along with additional driver examples from Dr. S.

These examples have been tested on Ubuntu 18.04. 

## Building

Ensure that you have kernel headers installed:

```
sudo apt update
sudo apt install linux-headers-$(uname -r)
```

Then, build the drivers:

```
make
```

## Testing

To test the memory and nothing drivers, see the article above.

### jiffies

To test the jiffies driver,

1. Create a device node:

   ```
   sudo mknod /dev/jiffies c 60 0
   sudo chmod 666 /dev/jiffies
   ```

1. Install the driver:

   ```
   sudo insmod jiffies.ko
   ```

1. Read the jiffies count from the driver:

   ```
   cat /dev/jiffies
   ```

1. Uninstall the driver:

   ```
   sudo rmmod jiffies
   ```

### emoticon

To test the emoticon driver, 

1. Create a device node:

   ```
   sudo mknod /dev/emoticon c 61 0
   sudo chmod 666 /dev/emoticon
   ```

1. Install the driver:

   ```
   sudo insmod emoticon.ko
   ```

1. Run the test program:

   ```
   ./emoticon_demo
   ```

1. Uninstall the driver:

   ```
   sudo rmmod emoticon
   ```

