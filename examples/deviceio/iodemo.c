#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

char buf[80] = "hello\n";
int main()
{
	int fd;
	int result;
	
	fd = open("/dev/tty1", O_RDWR);
	if (fd == -1) {
        perror("Unable to open device");
        exit(1);
    }
	result = write(fd, buf, strlen(buf));
	close(fd);
}
