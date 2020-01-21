xv6 is a re-implementation of Dennis Ritchie's and Ken Thompson's Unix
Version 6 (v6).  xv6 loosely follows the structure and style of v6,
but is implemented for a modern x86-based multiprocessor using ANSI C.

This version has been enhanced with the following:
* A halt system call and halt utility facilitate shutdowns from within xv6
* kernel.asm is generated to facilitate diagnosing kernel panics

# BUILDING AND RUNNING XV6

To build xv6 on an x86 ELF machine (like Linux or FreeBSD), run "make".
On non-x86 or non-ELF machines (like Mac OS, even on x86), you will
need to install a cross-compiler gcc suite capable of producing x86 ELF
binaries.  See http://pdos.csail.mit.edu/6.828/2007/tools.html.

To run xv6 in QEMU, run:

    make run


# DEBUGGING

To debug, start with a GDB debug server in one terminal:

    make debug

In another terminal, run:

    gdb

or

    gdb -tui