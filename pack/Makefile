CC=gcc
pack: main-pack.c pack.c
	$(CC) -o pack main-pack.c pack.c
unpack: main-unpack.c pack.c
	$(CC) -o unpack main-unpack.c pack.c
read-metadata: main-read-metadata.c pack.c
	$(CC) -o read-metadata main-read-metadata.c pack.c
write-metadata: main-write-metadata.c pack.c
	$(CC) -o write-metadata main-write-metadata.c pack.c
all:
	$(MAKE) pack
	$(MAKE) unpack
	$(MAKE) read-metadata
	$(MAKE) write-metadata
clean:
	rm pack
	rm unpack
	rm read-metadata
	rm write-metadata
