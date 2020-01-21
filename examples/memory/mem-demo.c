#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void show_address_space(char *title)
{
  char buf[80];

  sprintf(buf, "pmap -x %d", getpid());
  puts(title);
  system(buf);
}

int main(int argc, char *argv[])
{
  size_t size = 4194304;

  if (argc == 2)
    size = atoi(argv[1]);

  show_address_space("Before allocating memory");

  printf("Allocating %ld bytes...\n", size);
  char *mem = malloc(size);
  show_address_space("After allocating memory");

  size_t i;
  for (i = 0; i < size && i < 4096; ++i) {
    mem[i] = 3;
  }

  show_address_space("After touching up to 4096 bytes");

  for (; i < size; ++i) {
    mem[i] = 3;
  }


  show_address_space("After touching all bytes");


}
