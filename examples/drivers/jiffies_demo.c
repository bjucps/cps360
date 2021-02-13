#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

char buf[4] = "sad";
int main()
{
	int fd;
	int result;
	
	fd = open("/dev/jiffies", O_RDONLY);
	if (fd == -1) {
        perror("Unable to open device");
        exit(1);
    }
    do {
        result = read(fd, buf, sizeof(buf)-1);
        if (result >= 0) {
            buf[result] = '\0';
            printf("Read %d bytes: %s\n", result, buf);
        }
    } while (result > 0);
	close(fd);
}
