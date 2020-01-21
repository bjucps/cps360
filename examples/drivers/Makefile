# The next line defines the kernel modules to be built
obj-m = hello.o memory.o emoticon.o jiffies.o

# Obtain the version of the running kernel
KVERSION = $(shell uname -r)

all: modules emoticon_demo

modules:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules

emoticon_demo: emoticon_demo.c
	gcc emoticon_demo.c -oemoticon_demo
	
clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean
	rm emoticon_demo

