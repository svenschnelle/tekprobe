CFLAGS=-O2 -Wall -Wextra -ggdb -std=c17
LDFLAGS=-ljson-c
tekprobe:	tekprobe.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<
