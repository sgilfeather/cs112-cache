src = $(wildcard *.c)
obj = $(src:.c=.o)
CC = gcc -g
LDFLAGS = -lnsl

a.out: main.o cache.o sim_cache.o file_sys.o
	$(CC) -o $@ $^ $(LDFLAGS)

test: test_cache.o cache.o sim_cache.o file_sys.o
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) a.out