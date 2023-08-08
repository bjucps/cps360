EXAMPLES = threads_t1 cas

all: $(EXAMPLES)

%: %.c
	gcc -I include/ -o $@ $^ -lpthread

