#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "../database/fsdb.h"
#include "../answer/answer.h"
#include "../pack/pack.h"
#include "../config/config.h"
#include "../daemon/daemon.h"
#include "../parser/parser.h"

#include "aiqbd.h"

/**
 * function for registering a new AI problem
 * the function saves the problem into a new file, called p#id.aiq
 * in the database_dir folder. the problem status is set to 'N'
 * for new / not solved.
 * @param *problem @struct problem_t pointer to a new problem
 * @return problem id
 */
unsigned long _aiqbd_register_problem (struct problem_t * problem) {
	char buffer[32];
	unsigned long id, length;
	struct fsdb_t * atom;

	id = _config.auto_increment ++;

	printf ("changing dir to %s\n", _config.database_dir);
	if (chdir (_config.database_dir) < 0) {
		return -1;
		}
	sprintf (buffer, "p%lx.aiq", id);
	
	atom = (struct fsdb_t *) malloc (sizeof (struct fsdb_t));
	if (atom == NULL) {
		return -1;
		}
	atom->name = _concat_path (_config.database_dir, buffer);
	if (atom->name == NULL) {
		free (atom);
		return -1;
		}

	atom->header = (struct file_header_t *) malloc (sizeof (struct file_header_t));
	if (atom->header == NULL) {
		free (atom->name);
		free (atom);
		return -1;
		}

	memcpy (atom->header->prefix, "AIQB", 4);
	atom->header->id = id;
	atom->header->parent = (unsigned long) 0;
	atom->header->priority = 0;
	atom->header->status = 'N';

	printf ("packing problem: %s\n", atom->name);
	length = _problem_pack (buffer, problem, atom->header);
	if (!length) {
		free (atom->name);
		free (atom->header);
		free (atom);
		return -1;
		}

	atom->handler = NULL;
	atom->prev = atom;
	atom->next = atom;
	atom->root = NULL;

	_insert_db (&_config.problems, atom);

	return atom->header->id;
	}

unsigned long _aiqbd_register_answer (struct answer_t * answer) {
	answer->status = 'N';
	_insert_queue (&_config.answers, answer);
	return answer->id;
	}

void _aiqbd_list_problems (void) {
	_print_db (_config.problems);
	}

void _aiqbd_list_answers (void) {
	_print_queue (_config.answers);
	}

void * _aiqbd_handling_socket_client (void * arg) {
	int new_socket_fd = *((int *) arg);
	struct parser_handle_t handler;

	handler.register_problem = _aiqbd_register_problem;
	handler.register_answer = _aiqbd_register_answer;
	handler.request_answer = NULL; //_aiqbd_solve_answer;
	handler.list_problems = _aiqbd_list_problems;
	handler.list_answers = _aiqbd_list_answers;

	printf ("client accepted\n");

	_parser (new_socket_fd, &handler);

	printf ("client parser exit\n");

	close (new_socket_fd);

	pthread_exit (NULL);
	}

void * _aiqbd_handling_socket (void * arg) {
	socklen_t client_len;
	pthread_t client;
	pthread_attr_t attr;
	struct sockaddr_in server_addr, client_addr;
	int socket_fd, new_socket_fd, fd;
	char buffer[AIQBD_SOCKET_MAX_BUFFER];

	socket_fd = socket (AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0) {
		printf ("error opening socket\n");
		exit (1);
		}

	bzero ((char *) &server_addr, sizeof (server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons (_config.port);

	if (bind (socket_fd, (struct sockaddr *) &server_addr, sizeof (server_addr)) < 0) {
		printf ("error binding\n");
		pthread_exit (NULL);
		}
	listen (socket_fd, AIQBD_SOCKET_MAX_BACKLOG);

	fcntl (socket_fd, F_SETFL, O_NONBLOCK);

	if (pthread_attr_init (&attr) || pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED)) {
		printf ("error creating pthread\n");
		pthread_exit (NULL);
		}

	while (1) {
		if ((fd = accept (socket_fd, (struct sockaddr *) &client_addr, &client_len)) < 0) continue;
		new_socket_fd = fd;
		pthread_create (&client, &attr, _aiqbd_handling_socket_client, &new_socket_fd);
		}

	close (socket_fd);
	return NULL;
	}

void * _aiqbd_handling_problem (void * arg) {
	struct fsdb_t * problem = (struct fsdb_t *) arg;
	struct query_t query;

	printf ("solve problem\n");
	//_print_problem (problem);

	problem->header->status = 'S'; /** Solved */
	_problem_write_metadata (problem->name, problem->header);

	free (problem->handler);

	query.type = query_by_id;
	query.query.id = problem->header->id;

	problem = _extract_db (&_config.problem_queue, &query);
	_insert_db (&_config.problems, problem);
	return NULL;
	}

void * _aiqbd_handling_answer (void * arg) {
	struct answer_t * answer = (struct answer_t *) arg;

	printf ("solve answer\n");
	//_print_answer (answer);

	answer->status = 'S'; /** Solved */
	free (answer->handler);

	answer = _extract_queue (&_config.answer_queue, answer->id);
	_insert_queue (&_config.answers, answer);
	return NULL;
	}
