# The next line defines the kernel modules to be built
obj-m = hello.o memory.o devdriver.o emoticon.o jiffies.o klog.o 

# Obtain the version of the running kernel
KVERSION = $(shell uname -r)

all: modules emoticon_demo jiffies_demo devdriver_demo

modules:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules

devdriver_demo: devdriver_demo.c
	gcc devdriver_demo.c -odevdriver_demo

emoticon_demo: emoticon_demo.c
	gcc emoticon_demo.c -oemoticon_demo
	
clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean
	rm emoticon_demo jiffies_demo devdriver_demo

