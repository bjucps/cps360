// test with (and without): ./iodemo 2>&1 >/dev/null
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

char buf[80] = "hello\n";

// helper function to simplify
// running a command-line and
// getting its STDOUT as a 
// string (like $(...) in bash)
static char *runget(const char *cmd);

int main() {
	int ret = 1;
	int fd = -1;
	char *tty = NULL;

	// determine the active TTY name
	if ((tty = runget("tty")) == NULL) {
		fprintf(stderr, "can't run `tty` to find TTY name!\n");
		goto cleanup;
	}

	// open it for read/write access (or die)
	printf("opening '%s'...\n", tty);
	fd = open(tty, O_RDWR);
	if (fd == -1) {
        perror("Unable to open device");
        exit(1);
    }

	// write the specified message to it
	write(fd, buf, strlen(buf));
	ret = 0;
cleanup:
	if (tty) free(tty);
	if (fd >= 0) close(fd);
	return ret;
}

char *runget(const char *cmd) {
	char *ret = NULL;
	FILE *pf = NULL;
	char *buf = NULL;
	size_t blen = 0;

	// run command with STDOUT going to readable pipe
	if ((pf = popen(cmd, "r")) == NULL) {
		perror("popen");
		goto cleanup;
	}

	// slurp to EOF from pipe
	if (getdelim(&buf, &blen, EOF, pf) < 0) {
		perror("getdelim");
		goto cleanup;
	}

	// trim trailing WS
	size_t len = strlen(buf);
	while (isspace(buf[len-1])) {
		buf[len - 1] = '\0';
		--len;
	}

	// return string
	ret = buf;
	buf = NULL;
cleanup:
	if (buf) free(buf);
	if (pf) fclose(pf);
	return ret;
}


