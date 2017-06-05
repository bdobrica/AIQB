#ifndef ANSWER_ID_NEXT
#define ANSWER_ID_NEXT 0
#endif

/**
 * @struct with the problem hypothesis
 * @var size_x the size of the input data
 * @var size_y the size of the output data
 * @var *data the input|output vector
 * @var *next the next answer in queue
 * @var *prev the prev answer in queue
 * @var *problem the pointer to the atom in the problems database
 * @see struct fsdb_t
 */
struct answer_t {
	unsigned long id;
	unsigned long size_x;
	unsigned long size_y;
	unsigned char status;

	pthread_t * handler;

	double * data;

	struct answer_t * next;
	struct answer_t * prev;

	struct fsdb_t * problem;
	};

/**
 * insert an element into answer_t struct
 * @param *queue, pointer to queue structure
 */
void _insert_queue (struct answer_t ** queue, struct answer_t * answer);

/**
 * find an element from answer_t struct
 * @param *queue, pointer to queue structure
 * @param id, the answer id
 */
struct answer_t * _find_queue (struct answer_t * queue, unsigned long id);

/**
 * extract an element from answer_t struct
 * @param *queue, pointer to queue structure
 * @param id, the answer id
 * @return pointer to queue answer
 */
struct answer_t * _extract_queue (struct answer_t ** queue, unsigned long id);

/**
 * count the number of elements from fsqueue
 * @param *queue, pointer to queue structure
 * @return number of elements from queue
 */
unsigned long int _count_queue (struct answer_t * queue);

/**
 * get last inserted id from fsqueue queue
 * @param *queue, pointer to queue structure
 * @return the last id inserted (max of ids)
 */
unsigned long int _last_id_queue (struct answer_t * queue);
/**
 * frees the memory for only one answer
 * @param *answer, pointer to queue answer
 */

void _free_answer (struct answer_t * answer);

/**
 * closes the queue from answer_t struct
 * @param *queue, pointer to queue structure
 */
void _free_queue (struct answer_t * queue);

/**
 * debug function for displaying an answer
 * @param *answer pointer to a answer_t struct
 */
void _print_answer (struct answer_t * answer);

/**
 * debug function for printing the queue content
 * @param *queue, pointer to queue structure
 */
void _print_queue (struct answer_t * queue);
