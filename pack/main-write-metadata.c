#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "pack.h"

int main (int argc, char ** argv) {
	struct file_header_t metadata;
	unsigned long length;

	metadata.id = 1;
	metadata.parent = 2;
	metadata.priority = 3;
	metadata.status = 'A';

	printf ("prefix=%s\nid=%ld\nparent=%ld\npriority=%d\ntime=%ld\nstatus=%c\n", metadata.prefix, metadata.id, metadata.parent, metadata.priority, metadata.time, metadata.status);

	length = _problem_write_metadata (argv[1], &metadata);

	printf ("%ld bytes\n", length);
	}
