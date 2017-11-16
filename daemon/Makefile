CC=gcc
CFLAGS = -I.
DEPS = daemon.h ../config/config.h ../database/fsdb.h ../pack/pack.h
OBJ = main.o daemon.o ../config/config.o ../database/fsdb.o ../pack/pack.o

%.o: %.c $(DEPS)
		$(CC) -c -o $@ $< $(CFLAGS)
daemon: $(OBJ)
		gcc -o $@ $^ $(CFLAGS)

clean:
	rm daemon
