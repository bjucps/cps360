#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"

FILE *fopen(char *fname, char *mode)
{
  int fd = open(fname, O_RDONLY);
  if (fd == -1) {
    return NULL;
  }

  FILE *file = malloc(sizeof(FILE));
  file->fd = fd;
  return file;
}

char *fgets(char *buf, size_t size, FILE *file)
{
  size_t bytesread = 0;
  int shouldstop = 0;
  int result = 0;
  while (bytesread < size-1 && !shouldstop) {
    char c;
    result = read(file->fd, &c, 1);
    if (result == -1) {
      shouldstop = 1;
    } else if (result == 0) {
      shouldstop = 1;
    } else {
      buf[bytesread++] = c;
      if (c == '\n')
        shouldstop = 1;
    }
  }
  buf[bytesread] = '\0';
  if (result == -1) {
    return NULL;
  } 

  return buf;
}

void fclose(FILE *file) {
  close(file->fd);
  free(file);
}


char*
strcpy(char *s, char *t)
{
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
    ;
  return os;
}

int
strcmp(const char *p, const char *q)
{
  while(*p && *p == *q)
    p++, q++;
  return (uchar)*p - (uchar)*q;
}

uint
strlen(char *s)
{
  int n;

  for(n = 0; s[n]; n++)
    ;
  return n;
}

void*
memset(void *dst, int c, uint n)
{
  stosb(dst, c, n);
  return dst;
}

char*
strchr(const char *s, char c)
{
  for(; *s; s++)
    if(*s == c)
      return (char*)s;
  return 0;
}

char*
gets(char *buf, int max)
{
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
    cc = read(0, &c, 1);
    if(cc < 1)
      break;
    buf[i++] = c;
    if(c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
  return buf;
}

int
stat(char *n, struct stat *st)
{
  int fd;
  int r;

  fd = open(n, O_RDONLY);
  if(fd < 0)
    return -1;
  r = fstat(fd, st);
  close(fd);
  return r;
}

int
atoi(const char *s)
{
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
    n = n*10 + *s++ - '0';
  return n;
}

void*
memmove(void *vdst, void *vsrc, int n)
{
  char *dst, *src;
  
  dst = vdst;
  src = vsrc;
  while(n-- > 0)
    *dst++ = *src++;
  return vdst;
}
