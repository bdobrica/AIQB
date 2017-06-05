#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "answer.h"

/**
 * insert an element into answer_t struct
 * @param *queue, pointer to queue structure
 */
void _insert_queue (struct answer_t ** queue, struct answer_t * answer) {
	if (answer == NULL)
		return;
	if (*queue == NULL) {
		if (answer->id < 1)
			answer->id = 1;
		answer->prev = answer;
		answer->next = answer;
		*queue = answer;
		}
	else {
		if (answer->id < 1)
			answer->id = _last_id_queue (*queue) + 1;
		//answer->id = (*queue)->prev->id + 1;
		answer->next = (*queue);
		answer->prev = (*queue)->prev;
		(*queue)->prev->next = answer;
		(*queue)->prev = answer;
		}
	}

/**
 * find an element from answer_t struct
 * @param *queue, pointer to queue structure
 * @param id, the answer id
 */
struct answer_t * _find_queue (struct answer_t * queue, unsigned long id) {
	unsigned short int found = 0;
	struct answer_t * answer = queue;

	if (queue == NULL || id < 0)
		return NULL;
	if (id == 0)
		return queue;

	do {
		if (id == answer->id) {
			found = 1;
			break;
			}
		answer = answer->next;
		}
	while (answer != queue);
	if (!found)
		return NULL;
	return answer;
	}

/**
 * extract an element from answer_t struct
 * @param *queue, pointer to queue structure
 * @param id, the answer id
 * @return pointer to queue answer
 */
struct answer_t * _extract_queue (struct answer_t ** queue, unsigned long id) {
	struct answer_t * answer;

	if ((answer = _find_queue (*queue, id)) == NULL)
		return NULL;
		
	answer->prev->next = answer->next;
	answer->next->prev = answer->prev;

	if (*queue == answer)
		*queue = answer == answer->next ? NULL : answer->next;

	return answer;
	}

/**
 * count the number of elements from fsqueue
 * @param *queue, pointer to queue structure
 * @return number of elements from queue
 */
unsigned long int _count_queue (struct answer_t * queue) {
	unsigned long int count = 0;
	struct answer_t * answer = queue;

	if (answer == NULL)
		return 0;

	do {
		count ++;
		answer = answer->next;
		}
	while (answer != queue);

	return count;
	}

/**
 * get last inserted id from fsqueue queue
 * @param *queue, pointer to queue structure
 * @return the last id inserted (max of ids)
 */
unsigned long int _last_id_queue (struct answer_t * queue) {
	unsigned long last_id = 1;
	struct answer_t * answer = queue;

	do {
		last_id = last_id > answer->id ? last_id : answer->id;
		answer = answer->next;
		}
	while (answer != queue);

	return last_id;
	}

/**
 * frees the memory for only one answer
 * @param *answer, pointer to queue answer
 */

void _free_answer (struct answer_t * answer) {
	if (answer == NULL)
		return;
	free (answer->data);
	free (answer);
	}

/**
 * closes the queue from answer_t struct
 * @param *queue, pointer to queue structure
 */
void _free_queue (struct answer_t * queue) {
	struct answer_t * deleted, * answer;
	if (queue == NULL)
		return;
	answer = queue->next;
	while (answer != NULL) {
		deleted = answer;
		answer->prev->next = NULL;
		answer = answer->next;
		_free_answer (deleted);
		}
	}

/**
 * debug function for displaying an answer
 * @param *answer pointer to answer_t struct
 */
void _print_answer (struct answer_t * answer) {
	unsigned long c, d;
	printf ("answer\n\tsize_x = %ld\n\tsize_y = %ld\n",
		answer->size_x,
		answer->size_y);
	printf ("\tdata = [  ");
	for (c = 0; c < answer->size_x + answer->size_y; c++)
		printf ("%.2lf, ", *(answer->data + c));
	printf ("\b\b]\n");
	}

/**
 * debug function for printing the queue content
 * @param *queue, pointer to queue structure
 */
void _print_queue (struct answer_t * queue) {
	struct answer_t * answer = queue;

	if (answer == NULL) {
		printf ("void\n");
		return;
		}

	do {
		printf ("answer\n\tid:%ld\n\tstatus:%c\n", answer->id, answer->status);
		answer = answer->next;
		}
	while (answer != queue);
	}
