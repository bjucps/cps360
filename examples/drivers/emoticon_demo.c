#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

char buf[80] = "sad";
int main()
{
	int fd;
	int result;
	
	fd = open("/dev/emoticon", O_RDWR);
	if (fd == -1) {
        perror("Unable to open device");
        exit(1);
    }
	result = write(fd, buf, strlen(buf));
	if (result != strlen(buf)) {
		perror("problem writing to device");
		printf("result = %d\n", result);
	}
	read(fd, buf, sizeof(buf));
	puts(buf);
	close(fd);
}
