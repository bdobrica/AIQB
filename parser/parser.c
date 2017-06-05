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

/**
 * function that replies to some parsed data
 * @param fd, the file descriptor
 * @param state, the parser state @see reply_state_ct
 * @param data, the data passed to the replier
 */
void _reply (int fd, enum reply_state_ct state, union _reply_data data) {
	int n;
	char buffer[128];

	errno = 0;
	if (fd < 0 || fcntl (fd, F_GETFD) < 0 || errno == EBADF)
		return;
	switch (state) {
		case reply_problem_ok:
			sprintf (buffer, TOKEN_REPLY_PROBLEM_OK, data.id);
			n = write (fd, buffer, strlen (buffer));
			if (n < 0) {
				printf ("unable to write on descriptor\n");
				}
			break;
		case reply_answer_ok:
			sprintf (buffer, TOKEN_REPLY_ANSWER_OK, data.id);
			n = write (fd, buffer, strlen (buffer));
			if (n < 0) {
				printf ("unable to write on descriptor\n");
				}
			break;
		case reply_descriptor_error:
			sprintf (buffer, TOKEN_REPLY_DESCRIPTOR_ERROR);
			n = write (fd, buffer, strlen (buffer));
			if (n < 0) {
				printf ("unable to write to descriptor\n");
				}
			break;
		case reply_memory_error:
			sprintf (buffer, TOKEN_REPLY_MEMORY_ERROR);
			n = write (fd, buffer, strlen (buffer));
			if (n < 0) {
				printf ("unable to write to descriptor\n");
				}
			break;
		case reply_unknown_error:
			sprintf (buffer, TOKEN_REPLY_UNKNOWN_ERROR);
			n = write (fd, buffer, strlen (buffer));
			if (n < 0) {
				printf ("unable to write to descriptor\n");
				}
			break;
		default:
			sprintf (buffer, TOKEN_REPLY_IDLE);
			n = write (fd, buffer, strlen (buffer));
			if (n < 0) {
				printf ("unable to write to descriptor\n");
				}
			break;
		}
	}

/**
 * function that parses a stream of data from a file handler
 * @param fd, the file descriptor
 * @param *handler, pointer to the functions that will be applied on
 * 		parsed atoms
 */
void _parser (int fd, struct parser_handle_t * handler) {
	unsigned short int token_index, parser_index;
	enum token_state_ct token_state;
	enum parser_state_ct parser_state;
	unsigned short int token_flags[4];
	unsigned short int problem_tokens, answer_tokens;
	unsigned long int data_index;
	char c, token[128];
	unsigned long int db_id;

	struct answer_t * answer;
	struct problem_t * problem;

	union _reply_data reply_data;
	enum reply_state_ct reply_state;


	token_index = 0;
	token_state = token_idle;

	parser_index = 0;
	parser_state = parser_idle;

	bzero (token_flags, sizeof (token_flags));
	problem_tokens = 0;
	answer_tokens = 0;

	/** read a character at a time */
	while (read (fd, &c, 1)) {
		/** strip down comments */
		if (token_state == token_comment) {
			if (c == '\n' || c == '\r') {
				token_state = token_idle;
				token_index = 0;
				}
			continue;
			}
		if (c == ';' || c == '#' || c == '!') {
			token_state = token_comment;
			token_index = 0;
			continue;
			}
		/** based on characters, try to deduce what kind of data is passed */
		if (c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == ',')
			c = 0;
		else
			token_flags[3] ++;
		if (c >= '0' && c <= '9')
			token_flags[0] ++;
		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == ':')
			token_flags[1] ++;
		if (c == '.')
			token_flags[2] ++;
		token[token_index++] = c;

		/** if we have a complete token, analyse it */
		if (c == 0) {
			if (token_index > 1) {
				printf ("token: %s\n", token);
				if (token_flags[3] == token_flags[0]) {
					printf ("integer: %s\n", token);
					if (parser_state == parser_problem) {
						switch (parser_index) {
							case 0:
								problem->type = (unsigned int) atoi (token);
								break;
							case 1:
								problem->optm = (unsigned int) atoi (token);
								break;
							case 2:
								problem->rows = (unsigned long) atoi (token);
								break;
							case 3:
								problem->size_x = (unsigned long) atoi (token);
								break;
							case 4:
								problem->size_y = (unsigned long) atoi (token);
								problem->data = (double *) malloc (problem->rows * (problem->size_x + problem->size_y) * sizeof (double));

								if (problem->rows * (problem->size_x + problem->size_y) > 0 && problem->data == NULL) {
									_reply (fd, reply_memory_error, reply_data);
									free (problem->cv_data);
									free (problem->test_data);
									free (problem->state);
									free (problem);
									return;
									}
								break;
							case 5:
								problem->cv_size = (unsigned long) atoi (token);
								problem->cv_data = (unsigned long *) malloc (problem->cv_size * sizeof (unsigned long));

								if (problem->cv_size > 0 && problem->cv_data == NULL) {
									_reply (fd, reply_memory_error, reply_data);
									free (problem->test_data);
									free (problem->data);
									free (problem->state);
									free (problem);
									return;
									}
								break;
							case 6:
								problem->test_size = (unsigned long) atoi (token);
								problem->test_data = (unsigned long *) malloc (problem->test_size * sizeof (unsigned long));

								if (problem->test_size > 0 && problem->test_data == NULL) {
									_reply (fd, reply_memory_error, reply_data);
									free (problem->cv_data);
									free (problem->data);
									free (problem->state);
									free (problem);
									return;
									}
								break;
							case 7:
								problem->state_size = (unsigned long) atoi (token);
								problem->state = (double *) malloc (problem->state_size * sizeof (double));

								if (problem->state_size > 0 && problem->state == NULL) {
									_reply (fd, reply_memory_error, reply_data);
									free (problem->cv_data);
									free (problem->test_data);
									free (problem->data);
									free (problem);
									return;
									}
								break;
							}

						if (parser_index > 7 && parser_index < 8 + problem->cv_size) {
							data_index = parser_index - 8;
							*(problem->cv_data + data_index) = (unsigned long) atoi (token);
							}
						if (parser_index > 7 + problem->cv_size && parser_index < 8 + problem->cv_size + problem->test_size) {
							data_index = parser_index - 8 - problem->cv_size;
							*(problem->test_data + data_index) = (unsigned long) atoi (token);
							}

						parser_index ++;
						}
					if (parser_state == parser_answer) {
						switch (parser_index) {
							case 0:
								answer->size_x = (unsigned long) atoi (token);
								break;
							case 1:
								answer->size_y = (unsigned long) atoi (token);
								answer->data = (double *) malloc ((answer->size_x + answer->size_y) * sizeof (double));

								if (answer->size_x + answer->size_y > 0 && answer->data == NULL) {
									_reply (fd, reply_memory_error, reply_data);
									free (answer);
									return;
									}
								break;
							}
						parser_index ++;
						}
					}

				if (token_flags[3] == token_flags[0] + 1 && token_flags[2] == 1) {
					printf ("float: %s\n", token);
					if (parser_state == parser_problem) {
						if (parser_index > 7 + problem->cv_size + problem->test_size &&
							parser_index < 8 + problem->cv_size + problem->test_size + problem->rows * (problem->size_x + problem->size_y)) {
							data_index = parser_index - 8 - problem->cv_size - problem->test_size;

							*(problem->data + data_index) = (double) atof (token);
							}
						if (parser_index > 7 + problem->cv_size + problem->test_size + problem->rows * (problem->size_x + problem->size_y) &&
							parser_index < 8 + problem->cv_size + problem->test_size + problem->rows * (problem->size_x + problem->size_y) + problem->state_size) {
							data_index = parser_index - 8 - problem->cv_size - problem->test_size - problem->rows * (problem->size_x + problem->size_y);

							*(problem->state + data_index) = (double) atof (token);
							}

						parser_index ++;

						if (parser_index == 8 + problem->cv_size + problem->test_size + problem->rows * (problem->size_x + problem->size_y) + problem->state_size) {
							reply_state = reply_problem_ok;
							reply_data.id = 1;
							_reply (fd, reply_state, reply_data);

							_print_problem (problem);
							parser_state = parser_idle;
							parser_index = 0;
							}
						}
					if (parser_state == parser_answer) {
						if (parser_index > 1 && parser_index < 2 + answer->size_x + answer->size_y) {
							data_index = parser_index - 2;
							*(answer->data + data_index) = (double) atof (token);
							}

						parser_index ++;

						if (parser_index == 2 + answer->size_x + answer->size_y) {
							reply_state = reply_answer_ok;
							reply_data.id = 1;
							_reply (fd, reply_state, reply_data);

							_print_answer (answer);
							parser_state = parser_idle;
							parser_index = 0;
							}
						}
					}

				if (token_flags[3] == token_flags[1]) {
					printf ("command: %s\n", token);

					if (strcmp (token, TOKEN_REGISTER_PROBLEM) == 0) {
						parser_state = parser_problem;
						parser_index = 0;

						problem = (struct problem_t *) malloc (sizeof (struct problem_t));
						problem->cv_data = NULL;
						problem->test_data = NULL;
						problem->data = NULL;
						problem->state = NULL;

						if (problem == NULL) {
							_reply (fd, reply_memory_error, reply_data);
							return;
							}
						if (handler != NULL && handler->register_problem != NULL) {
							db_id = handler->register_problem (problem);
							}
						}

					if (strcmp (token, TOKEN_REGISTER_ANSWER) == 0) {
						parser_state = parser_answer;
						parser_index = 0;

						answer = (struct answer_t *) malloc (sizeof (struct answer_t));
						answer->data = NULL;

						if (problem == NULL) {
							_reply (fd, reply_memory_error, reply_data);
							return;
							}
						if (handler != NULL && handler->register_answer != NULL) {
							db_id = handler->register_answer (answer);
							}
						}

					if (strcmp (token, TOKEN_REQUEST_ANSWER) == 0) {
						}

					if (strcmp (token, TOKEN_REQUEST_STATE) == 0) {
						}

					if (strcmp (token, TOKEN_REQUEST_CLOSE) == 0) {
						switch (parser_state) {
							case parser_problem:
								free (problem->cv_data);
								free (problem->test_data);
								free (problem->data);
								free (problem->state);
								free (problem);
								break;
							case parser_answer:
								free (answer->data);
								free (answer);
								break;
							}
						return;
						}
					}

				bzero (token_flags, sizeof (token_flags));
				}
			token_index = 0;
			continue;
			}
		}
	}
