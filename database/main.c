#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

#include "../pack/pack.h"
#include "fsdb.h"

int main (int argc, char ** argv) {
	unsigned long count;
	struct fsdb_t * db;
	db = _load_db (argv[1]);
	_print_db (db);

	count = _count_db (db);

	printf ("initial: %ld\n", count);

	_free_db (db);
	}
