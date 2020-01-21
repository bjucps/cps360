
ALL = barrier rendezvous
COMMON = common_threads.h

all: $(ALL)

clean:
	rm -f $(ALL) *~

barrier: barrier.c $(COMMON)
	gcc -o barrier barrier.c -Wall -pthread -O

rendezvous: rendezvous.c $(COMMON)
	gcc -o rendezvous rendezvous.c -Wall -pthread -O

