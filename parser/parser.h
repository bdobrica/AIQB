#ifndef PARSER_TOKENS
#define PARSER_TOKENS

#define TOKEN_REGISTER_PROBLEM		"reg:problem"
#define TOKEN_REGISTER_ANSWER		"reg:answer"
#define TOKEN_REQUEST_SOLUTION		"req:solution"
#define TOKEN_REQUEST_STATE		"req:state"
#define TOKEN_REQUEST_CLOSE		"req:close"
#define TOKEN_LIST_PROBLEMS		"lst:problems"
#define TOKEN_LIST_ANSWERS		"lst:answers"
#define TOKEN_DELETE_ANSWER		"dlt:answer"

#endif

#ifndef REPLY_TOKENS
#define REPLY_TOKENS

#define TOKEN_REPLY_IDLE		"ok\n"
#define TOKEN_REPLY_UNKNOWN_ERROR	"err:unknown\n"
#define TOKEN_REPLY_MEMORY_ERROR	"err:memory\n"
#define TOKEN_REPLY_DESCRIPTOR_ERROR	"err:descriptor\n"
#define TOKEN_REPLY_PROBLEM_OK		"ok:problem:%ld\n"
#define TOKEN_REPLY_ANSWER_OK		"ok:answer:%ld\n"
#define TOKEN_REPLY_SOLUTION_OK		"ok:solution:"
#define TOKEN_REPLY_NOT_READY		"wrn:notready\n"
#define TOKEN_REPLY_SOLUTION		"ok:solution:"
#define TOKEN_REPLY_SOLUTION_ATOM	"%.2lf "
#endif

/**
 * @enum token_state_ct
 * token state constants: used to ignore comments
 */
enum token_state_ct {
	token_idle,
	token_comment
	};

/**
 * @enum parser_state_ct
 * the parser is a state-machine. constants that
 * define the states
 */
enum parser_state_ct {
	parser_idle,
	parser_problem,
	parser_answer,
	parser_solution,
	parser_delete_answer
	};

/**
 * @union _reply_data
 * data-structure to hold the passed arguments to the
 * replies. as the passed data is heterogenous, we use
 * see @reply_state_ct and _reply_data structures
 */
union _reply_data {
	unsigned long id;
	struct answer_t * answer;
	};

/**
 * @enum _reply_data
 * constants that define the reply state
 */
enum reply_state_ct {
	reply_idle,
	reply_unknown_error,
	reply_memory_error,
	reply_descriptor_error,
	reply_problem_ok,
	reply_answer_ok,
	reply_solution,
	reply_not_ready
	};

/**
 * @struct parser_handle_t
 * each member defines a pointer to a function that is applied
 * on the parsed atom
 */
struct parser_handle_t {
	unsigned long (* register_problem) (struct problem_t *);
	unsigned long (* register_answer) (struct answer_t *);
	unsigned long (* request_answer) (struct answer_t *);
	void (* list_problems) (void);
	void (* list_answers) (void);
	};

/**
 * function that replies to some parsed data
 * @param fd, the file descriptor
 * @param state, the parser state @see reply_state_ct
 * @param data, the data passed to the replier
 */
void _reply (int fd, enum reply_state_ct state, union _reply_data data);

/**
 * function that parses a stream of data from a file handler
 * @param fd, the file descriptor
 * @param *handler, pointer to the functions that will be applied on
 * 		parsed atoms
 */
void _parser (int fd, struct parser_handle_t * handler);
