#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "../daemon/daemon.h"
#include "../pack/pack.h"
#include "../database/fsdb.h"

#include "../answer/answer.h"
#include "config.h"

void _trim (char chr, char * str) {
	int c = 0;
	int l = strlen (str);

	while (*(str + c) == chr && c < l)
		c ++;
	if (c < l)
		memmove (str, str + c, l - c + 1);

	c = l - c - 1;
	while (*(str + c) == chr && c >= 0)
		*(str + c--) = 0;
	}

int _init_config (struct config_t * config) {
	/** string */
	config->config_dir = NULL;
	config->config_file = NULL;
	config->database_dir = NULL;
	config->run_dir = NULL;
	config->pid_file = NULL;
	/** fsdb_t * */
	config->problems = NULL;
	config->problem_queue = NULL;
	/** answer_t * */
	config->answers = NULL;
	config->answer_queue = NULL;
	/** int */
	config->db = 0;
	config->queue = 0;
	config->queue_size = 0;
	config->no_daemon - 0;
	config->problem_queue_size = 8;
	config->answer_queue_size = 8;
	config->auto_increment = 0;
	config->port = 0;
	/** file descriptor */
	config->pid_file_d = 0;
	return 1;
	}

void _free_config (struct config_t * config) {
	/** string */
	free (config->config_dir);
	free (config->config_file);
	free (config->database_dir);
	free (config->run_dir);
	free (config->pid_file);
	/** fsdb_t * */
	_free_db (config->problems);
	_free_db (config->problem_queue);
	_free_db (config->db);
	_free_db (config->queue);
	/** answer_t * */
	//_free_queue (config->answers);
	//_free_queue (config->answer_queue);
	
	/** int */
	/** file descriptor */
	close (config->pid_file_d);
	}

int _assign_config (char * key, char * value, struct config_t * config) {
	int l;
	if (strcmp (key, CONFIG_TOKEN_S_CONFIG_DIR) == 0 || strcmp (key, CONFIG_TOKEN_CONFIG_DIR) == 0) {
		if (value == NULL)
			return 1;
		_trim ('"', value);
		_trim ('\'', value);
		l = strlen (value);
		if (l > 0) {
			config->config_dir = (char *) realloc (config->config_dir, strlen (value) * sizeof (char));
			memcpy (config->config_dir, value, strlen (value));
			}
		return 1;
		}
	if (strcmp (key, CONFIG_TOKEN_S_CONFIG_FILE) == 0 || strcmp (key, CONFIG_TOKEN_CONFIG_FILE) == 0) {
		if (value == NULL)
			return 1;
		_trim ('"', value);
		_trim ('\'', value);
		l = strlen (value);
		if (l > 0) {
			config->config_file = (char *) realloc (config->config_file, strlen (value) * sizeof (char));
			memcpy (config->config_file, value, strlen (value));
			}
		return 1;
		}
	if (strcmp (key, CONFIG_TOKEN_S_DATABASE_DIR) == 0 || strcmp (key, CONFIG_TOKEN_DATABASE_DIR) == 0) {
		if (value == NULL)
			return 1;
		_trim ('"', value);
		_trim ('\'', value);
		l = strlen (value);
		if (l > 0) {
			config->database_dir = (char *) realloc (config->database_dir, strlen (value) * sizeof (char));
			memcpy (config->database_dir, value, strlen (value));
			}
		return 1;
		}
	if (strcmp (key, CONFIG_TOKEN_S_RUN_DIR) == 0 || strcmp (key, CONFIG_TOKEN_RUN_DIR) == 0) {
		if (value == NULL)
			return 1;
		_trim ('"', value);
		_trim ('\'', value);
		l = strlen (value);
		if (l > 0) {
			config->run_dir = (char *) realloc (config->run_dir, strlen (value) * sizeof (char));
			memcpy (config->run_dir, value, strlen (value));
			}
		return 1;
		}
	if (strcmp (key, CONFIG_TOKEN_S_PID_FILE) == 0 || strcmp (key, CONFIG_TOKEN_PID_FILE) == 0) {
		if (value == NULL)
			return 1;
		_trim ('"', value);
		_trim ('\'', value);
		l = strlen (value);
		if (l > 0) {
			config->pid_file = (char *) realloc (config->pid_file, strlen (value) * sizeof (char));
			memcpy (config->pid_file, value, strlen (value));
			}
		return 1;
		}
	if (strcmp (key, CONFIG_TOKEN_S_QUEUE_SIZE) == 0 || strcmp (key, CONFIG_TOKEN_QUEUE_SIZE) == 0) {
		if (value == NULL)
			return 1;
		_trim ('"', value);
		_trim ('\'', value);
		l = strlen (value);
		if (l > 0) {
			config->problem_queue_size = atoi (value);
			}
		return 1;
		}
	if (strcmp (key, CONFIG_TOKEN_S_ANSWERS_SIZE) == 0 || strcmp (key, CONFIG_TOKEN_ANSWERS_SIZE) == 0) {
		if (value == NULL)
			return 1;
		_trim ('"', value);
		_trim ('\'', value);
		l = strlen (value);
		if (l > 0) {
			config->answer_queue_size = atoi (value);
			}
		return 1;
		}
	if (strcmp (key, CONFIG_TOKEN_S_NO_DAEMON) == 0 || strcmp (key, CONFIG_TOKEN_NO_DAEMON) == 0) {
		config->no_daemon = 1;
		return 1;
		}
	if (strcmp (key, CONFIG_TOKEN_S_SOCKET) == 0 || strcmp (key, CONFIG_TOKEN_SOCKET) == 0) {
		if (value == NULL)
			return 1;
		_trim ('"', value);
		_trim ('\'', value);
		l = strlen (value);
		if (l > 0) {
			config->port = atoi (value);
			}
		return 1;
		}
	return 0;
	}

void _parse_args (int argc, char ** argv, struct config_t * config) {
	int c = 1;
	while (c < argc) {
		if (**(argv + c) == '-') {
			_trim ('-', *(argv + c));
			if (c + 1 < argc) {
				if (**(argv + c + 1) == '-') {
					if (_assign_config (*(argv + c), NULL, config)) {
						c += 1;
						continue;
						}
					}
				else {
					if (_assign_config (*(argv + c), *(argv + c + 1), config)) {
						c += 2;
						continue;
						}
					}
				}
			}
		c ++;
		}
	}

void _parse_file (struct config_t * config) {
	int l;
	char * eq, * buffer;
	FILE * f;
	
	if ((f = fopen (config->config_file == NULL ? CONFIG_DEFAULT_CONFIG_FILE : config->config_file, "r")) == NULL) return;

	buffer = (char *) malloc (CONFIG_MAX_BUFFER * sizeof (char));

	while (!feof (f)) {
		if (NULL != fgets (buffer, CONFIG_MAX_BUFFER, f)) {
			if ((l = strlen (buffer)) == 0) continue;

			_trim (' ', buffer);
			_trim ('\t', buffer);
			_trim ('\r', buffer);
			_trim ('\n', buffer);

			if (*buffer == '#' || *buffer == ';') continue;
			if (*buffer == '[') continue;

			eq = strstr (buffer, "=");
			if (eq == NULL) continue;

			*eq = 0;
			_trim (' ', buffer);
			_trim ('\t', buffer);
			_trim (' ', eq + 1);
			_trim ('\t', eq + 1);

			_assign_config (buffer, eq + 1, config);
			}
		}
	
	free (buffer);
	
	close (f);
	}

void _print_config (struct config_t * config) {
	printf ("Config Dir:	%s\n", config->config_dir);
	printf ("Config File:	%s\n", config->config_file);
	printf ("Database Dir:	%s\n", config->database_dir);
	printf ("Run Dir:	%s\n", config->run_dir);
	printf ("Pid File:	%s\n", config->pid_file);
	printf ("Daemonize?:	%s\n", config->no_daemon ? "no" : "yes");
	printf ("Problem Size:	%d\n", config->problem_queue_size);
	printf ("Answer Size:	%d\n", config->answer_queue_size);
	}
