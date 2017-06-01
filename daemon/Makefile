CC=gcc
daemon: main.c daemon.c ../config/config.c ../database/fsdb.c ../pack/pack.c
	$(CC) -o daemon main.c daemon.c ../config/config.c ../database/fsdb.c ../pack/pack.c
clean:
	rm daemon
