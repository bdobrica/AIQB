/*
 * #include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
*/
#include <stdio.h>
#include <stdint.h>
#include <syslog.h>

#include "../pack/pack.h"
#include "../database/fsdb.h"
#include "../config/config.h"

#include "daemon.h"

int main (int argc, char ** argv) {
	struct fsdb_t * atom;
	/**
	 * debug logging
	 * setlogmask(LOG_UPTO(LOG_DEBUG));
	 * openlog(DAEMON_NAME, LOG_CONS, LOG_USER)
	 */

	_init_config (&_config);
	_parse_args (argc, argv, &_config);
	_parse_file (&_config);

	/** logging */
	setlogmask (LOG_UPTO (LOG_INFO));
	openlog (DAEMON_NAME, LOG_CONS | LOG_PERROR, LOG_USER);

	syslog (LOG_INFO, "Daemon starting up");

	/** deamonize */
	if (!_config.no_daemon)
		_daemonize (_config.run_dir, _config.pid_file);

	_config.db = _load_db (_config.database_dir);

	syslog (LOG_INFO, "Daemon running");

	while (1) {
	/**
	 * take a problem from the main list and move it to the queue
	 * wait until it is solved by a pthread
	 */
//		if (_config.db->header->status == 'A') {
			if (_count_db (_config.queue) < _config.queue_size) {
				printf ("extract atom\n");
				atom = _extract_db (&_config.db, NULL);
				printf ("insert atom\n");
				_insert_db (&_config.queue, atom);
				}
//			}
		printf ("queue: \n");
		_print_db (_config.queue);
		printf ("\ndatabase: \n");
		_print_db (_config.db);
		printf ("============\n");
		if (_config.db)
			_config.db = _config.db->next;
		sleep (1);
		}
	}
