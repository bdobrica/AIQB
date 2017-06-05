#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

//#include "../pack/pack.h"
//#include "../database/fsdb.h"
#include "config.h"

int main (int argc, char ** argv) {
	struct config_t config;

	_init_config (&config);
	_parse_args (argc, argv, &config);
	_parse_file (&config);

	printf ("etc = %s\nfile = %s\ndb = %s\nport = %d\n", config.config_dir, config.config_file, config.database_dir, config.port);

	_free_config (&config);
	}
