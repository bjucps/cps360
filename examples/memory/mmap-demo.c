// mmap-demo.c
// - Demonstrates usage of mmap to read a file
// - To try it out:
//      gcc mmap-demo.c -ommap-demo
//      ./mmap-demo mmap-demo.c 50
//   ... should display the first 50 bytes of mmap-demo.c

#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char * argv[]){
    if(argc != 3) {
        fprintf(stderr, "Usage: mmap-demo <filename> <numbytes>\n");
        exit(1);
    }

    int fd;
    char *fname = argv[1];

    fd = open (fname, O_RDWR);
    if(fd == -1){
        fprintf(stderr, "Could not open file %s: %s\n", fname, strerror(errno));
        exit(1);  
    }

    int len = atoi(argv[2]);

    char *p = mmap (0, len, PROT_READ, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED) {
        printf("Could not map file %s: %s\n", fname, strerror(errno));
        exit(1); 
    }


    for (int i = 0; i < len; ++i){
        putchar(p[i]);
    }

    munmap(p, len);
    close (fd);
  
}
