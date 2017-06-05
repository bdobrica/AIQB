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
#include <stdlib.h>
#include <stdint.h>
#include <syslog.h>

#include <pthread.h>

#include "../pack/pack.h"
#include "../answer/answer.h"
#include "../database/fsdb.h"
#include "../config/config.h"
#include "../daemon/daemon.h"

#include "aiqbd.h"

int main (int argc, char ** argv) {
	int s;
	pthread_t socket;
	pthread_attr_t attr;
	struct fsdb_t * problem;
	struct answer_t * answer;
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

	/** load problems' db */
	_config.problems = _load_db (_config.database_dir);
	_config.auto_increment = _last_id_db (_config.problems) + 1;

	/** set pthread attr */
	if (pthread_attr_init (&attr) || pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED)) {
		exit (1);
		}

	syslog (LOG_INFO, "Daemon starting up");

	/** deamonize */
	if (!_config.no_daemon)
		_daemonize (_config.run_dir, _config.pid_file);

	syslog (LOG_INFO, "Daemon running");

	if (pthread_create (&socket, &attr, &_aiqbd_handling_socket, NULL)) {
		/** no communication! */
		}

	while (1) {
		if (_count_db (_config.problem_queue) < _config.problem_queue_size) {
			problem = _extract_db (&_config.problems, NULL);
			if (problem != NULL) {
				if (problem->header->status == 'N') {
					problem->header->status = 'I'; /** In progress */
					problem->handler = (pthread_t *) malloc (sizeof (pthread_t));
					_insert_db (&_config.problem_queue, problem);
					if (pthread_create (problem->handler, &attr, &_aiqbd_handling_problem, problem)) {
						/** no problem */
						}
					}
				else {
					_insert_db (&_config.problems, problem);
					}
				}
			}
		if (_count_queue (_config.answer_queue) < _config.answer_queue_size) {
			answer = _extract_queue (&_config.answers, 0);
			if (answer != NULL) {
				if (answer->status == 'N') {
					answer->status = 'I'; /** In progress */
					answer->handler = (pthread_t *) malloc (sizeof (pthread_t));
					_insert_queue (&_config.answer_queue, answer);
					if (pthread_create (answer->handler, &attr, &_aiqbd_handling_answer, answer)) {
						/** no answer */
						}
					}
				else {
					_insert_queue (&_config.answers, answer);
					}
				}
			}

		if (_config.problems != NULL) {
			//_config.problems = _config.problems->next;
			}
		if (_config.answers != NULL) {
			//_config.answers = _config.answers->next;
			}

		sleep (1);
		}
	}
