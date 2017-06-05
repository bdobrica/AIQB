#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "../answer/answer.h"
#include "../pack/pack.h"
#include "parser.h"

int main (int argc, char ** argv) {
	int fd;
	struct parser_handle_t * parser_handle;

	fd = open (argv[1], O_RDONLY);

	if (fd < 0) {
		printf ("error opening file\n");
		exit (1);
		}

	_parser (fd, parser_handle);

	close (fd);
	return 0;
	}
