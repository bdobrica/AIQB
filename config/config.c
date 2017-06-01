#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "../pack/pack.h"
#include "../database/fsdb.h"
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
	config->db = NULL;
	config->queue = NULL;
	/** int */
	config->no_daemon - 0;
	config->queue_size = 8;
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
	_free_db (config->db);
	_free_db (config->queue);
	/** int */
	/** file descriptor */
	close (config->pid_file_d);
	}

int _assign_config (char * key, char * value, struct config_t * config) {
	int l;
	if (strcmp (key, "e") == 0 || strcmp (key, "config_dir") == 0) {
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
	if (strcmp (key, "c") == 0 || strcmp (key, "config_file") == 0) {
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
	if (strcmp (key, "d") == 0 || strcmp (key, "database_dir") == 0) {
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
	if (strcmp (key, "r") == 0 || strcmp (key, "run_dir") == 0) {
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
	if (strcmp (key, "p") == 0 || strcmp (key, "pid_file") == 0) {
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
	if (strcmp (key, "q") == 0 || strcmp (key, "queue_size") == 0) {
		if (value == NULL)
			return 1;
		_trim ('"', value);
		_trim ('\'', value);
		l = strlen (value);
		if (l > 0) {
			config->queue_size = atoi (value);
			}
		return 1;
		}
	if (strcmp (key, "n") == 0 || strcmp (key, "no_deamon") == 0) {
		config->no_daemon = 1;
		return 1;
		}
	if (strcmp (key, "s") == 0 || strcmp (key, "socket") == 0) {
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
	
	if ((f = fopen (config->config_file == NULL ? CONFIG_FILE : config->config_file, "r")) == NULL) return;

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
