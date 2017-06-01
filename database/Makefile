CC=gcc
fsdb: main.c fsdb.c ../pack/pack.c
	$(CC) -o fsdb main.c fsdb.c ../pack/pack.c
all:
	$(MAKE) clean
	$(MAKE) fsdb
clean:
	rm fsdb
