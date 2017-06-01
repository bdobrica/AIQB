#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../pack/pack.h"

enum token_state_t {
	token_idle,
	token_comment
	};

enum parser_state_t {
	parser_idle,
	parser_problem,
	parser_answer
	};

struct parser_handle_t {
	unsigned long (* register_problem) (struct problem_t *);
	unsigned long (* register_answer) (struct answer_t *);
	};

/*
struct answer_t {
	unsigned long size_x;
	unsigned long size_y;
	double * data;
	};

struct problem_t {
	unsigned int type;
	unsigned int optm;

	unsigned long rows;
	unsigned long size_x;
	unsigned long size_y;

	unsigned long cv_size;
	unsigned long test_size;

	unsigned long state_size;

	unsigned long * cv_data;
	unsigned long * test_data;

	double * data;
	double * state;
	};
*/

void _parser (int fd, struct parser_handle_t * handler) {
	unsigned short int token_index, parser_index;
	enum token_state_t token_state;
	enum parser_state_t parser_state;
	unsigned short int token_flags[4];
	unsigned short int problem_tokens, answer_tokens;
	unsigned long data_index;
	char c, token[128];

	struct answer_t * answer;
	struct problem_t * problem;

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
		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_'))
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
								break;
							case 5:
								problem->cv_size = (unsigned long) atoi (token);
								problem->cv_data = (unsigned long *) malloc (problem->cv_size * sizeof (unsigned long));
								break;
							case 6:
								problem->test_size = (unsigned long) atoi (token);
								problem->test_data = (unsigned long *) malloc (problem->test_size * sizeof (unsigned long));
								break;
							case 7:
								problem->state_size = (unsigned long) atoi (token);
								problem->state = (double *) malloc (problem->state_size * sizeof (double));
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
							_print_answer (answer);
							parser_state = parser_idle;
							parser_index = 0;
							}
						}
					}

				if (token_flags[3] == token_flags[1]) {
					printf ("command: %s\n", token);

					if (strcmp (token, "register_problem") == 0) {
						parser_state = parser_problem;
						parser_index = 0;

						problem = (struct problem_t *) malloc (sizeof (struct problem_t));
						if (handler != NULL && handler->register_problem != NULL) {
							handler->register_problem (problem);
							}
						}

					if (strcmp (token, "register_answer") == 0) {
						parser_state = parser_answer;
						parser_index = 0;

						answer = (struct answer_t *) malloc (sizeof (struct answer_t));

						if (handler != NULL && handler->register_answer != NULL) {
							handler->register_answer (answer);
							}
						}
					}

				bzero (token_flags, sizeof (token_flags));
				}
			token_index = 0;
			continue;
			}
		}
	}

int main (int argc, char ** argv) {
	int fd;

	fd = open (argv[1], O_RDONLY);

	if (fd < 0) {
		printf ("error opening file\n");
		exit (1);
		}

	_parser (fd);

	close (fd);
	return 0;
	}
