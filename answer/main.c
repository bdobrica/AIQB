#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "answer.h"

int main (int argc, char ** argv) {
	struct answer_t * queue = NULL, * answer;

	answer = (struct answer_t *) malloc (sizeof (struct answer_t *));
	answer->size_x = 2;
	answer->size_y = 2;
	answer->data = (double *) malloc (4 * sizeof (double));
	
	_insert_queue (&queue, answer);

	answer = (struct answer_t *) malloc (sizeof (struct answer_t *));
	answer->size_x = 2;
	answer->size_y = 2;
	answer->data = (double *) malloc (4 * sizeof (double));

	_insert_queue (&queue, answer);

	answer = (struct answer_t *) malloc (sizeof (struct answer_t *));
	answer->size_x = 2;
	answer->size_y = 2;
	answer->data = (double *) malloc (4 * sizeof (double));

	_insert_queue (&queue, answer);

	_print_queue (queue);

	answer = _extract_queue (&queue, ANSWER_ID_NEXT);

	printf ("extracted: \n");
	_print_answer (answer);
	_print_queue (queue);

	answer = _extract_queue (&queue, ANSWER_ID_NEXT);

	printf ("extracted: \n");
	_print_answer (answer);
	_print_queue (queue);

	_free_queue (queue);

	return 0;
	}
