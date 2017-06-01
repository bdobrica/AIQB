#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "pack.h"

int main (int argc, char ** argv) {
	struct file_header_t metadata;
	unsigned long length;

	length = _problem_read_metadata (argv[1], &metadata);

	printf ("file_header_t = %ld\n", sizeof (union _file_header));

	printf ("prefix=%s\nid=%ld\nparent=%ld\npriority=%d\ntime=%ld\nstatus=%c\n", metadata.prefix, metadata.id, metadata.parent, metadata.priority, metadata.time, metadata.status);

	printf ("%ld bytes\n", length);
	}
