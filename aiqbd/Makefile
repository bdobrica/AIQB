CC=gcc
daemon: main.c aiqbd.c ../daemon/daemon.c ../config/config.c ../answer/answer.c ../database/fsdb.c ../pack/pack.c ../parser/parser.c
	$(CC) -lpthread -o aiqbd main.c aiqbd.c ../daemon/daemon.c ../config/config.c ../answer/answer.c ../database/fsdb.c ../pack/pack.c ../parser/parser.c
clean:
	rm aiqbd
