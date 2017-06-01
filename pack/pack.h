/**
 * the AI problem file structure:
 * header: 4 bytes | TYPE (2 bytes) | OPTIMIZATION (2 bytes) |
 * data size: 8 bytes | ROWS (4 bytes) | INPUT SIZE (2 bytes) | OUTPUT SIZE (2 bytes) |
 * cross validation size: 4 bytes | ROWS (4 bytes) |
 * test size: 4 bytes | ROWS (4 bytes) |
 * state size: 4 bytes | LENGTH (4 bytes) |
 * cross validation: | INDICES (unsigned long, ...) |
 * test: | INDICES (unsigned long, ...) |
 * data: | DATA (double, ...) |
 * state: | STATE (double, ...) |
 */

/**
 * struct for keeping file metadata, not implemented
 * should add the binary data at the begining of the AI problem file
 */
struct file_header_t {
	char prefix[4];
	unsigned long id;
	unsigned long parent;
	unsigned short int priority;
	unsigned long time;
	unsigned char status;
	};
/**
 * union for reading file metadata, not implemented
 */
union _file_header {
	struct file_header_t header;
	uint8_t bin[48];
	};
/**
 * @struct with the problem hypothesis
 * @var size_x the size of the input data
 * @var size_y the size of the output data
 * @var * data the input|output vector
 */
struct answer_t {
	unsigned long size_x;
	unsigned long size_y;
	double * data;
	};
/**
 * @struct holds an AI problem
 * @var type the type of the problem
 * @var optm the type of the optimizer used (nelder-mead, etc)
 * @var rows the number of training data points
 * @var size_x the size of an input vector
 * @var size_y the size of an output vector
 * @var cv_size the size of the cross-validation data
 * @var test_size the size of the test data
 * @var *cv_data the vector of indices for cross-validation data
 * @var *test_data the vector of indices for test data
 * @var *data the training points
 * @var *state the processed training points
 */
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
/**
 * @struct ensure translation between datatypes
 * has same structure as
 * @see @struct problem_t
 * doesn't hold any data, just meta-data
 */
struct _problem_t {
	uint16_t type;
	uint16_t optm;

	uint64_t rows;
	uint32_t size_x;
	uint32_t size_y;

	uint64_t cv_size;
	uint64_t test_size;
	uint64_t state_size;
	};
/**
 * @union used in reading and writing the file header
 * used to ensure transition between data-types: each header will be 48 bytes long
 */
union _problem_header {
	struct _problem_t problem;
	uint8_t bin[48];
	};
/**
 * @union used in reading and writing the data
 * used to ensure transition between data-types: each atom will be 8 bytes long
 */
union _problem_atom {
	double data;
	unsigned long index;
	uint8_t bin[8];
	};
/**
 * function for packing an IA problem in a file
 * @param *name the name of the file
 * @param *problem pointer to the problem datastructure
 * @return length of the written file
 */
unsigned long _problem_pack (unsigned char * name, struct problem_t * problem);
/**
 * function for unpacking an AI problem from a file
 * @param *name the name of the file
 * @param *problem a pointer to a problem datastructure which will be populated with data
 * @return length of the read data
 */
unsigned long _problem_unpack (unsigned char * name, struct problem_t * problem);
/**
 * function for unpacking only the metadata of AI problem
 * @param *name the name of the file
 * @param *metadata the metadata structure
 * @return length of the read data
 */
unsigned short _problem_read_metadata (unsigned char * name, struct file_header_t * metadata);
/**
 * function for packing only the metadata of AI problem
 * @param *name the name of the file
 * @param *metadata the metadata structure
 * @return length of the written data
 */
unsigned short _problem_write_metadata (unsigned char * name, struct file_header_t * metadata);
/**
 * debug function for displaying a problem
 * @param *problem pointer to a problem_t struct
 */
void _print_problem (struct problem_t * problem);
/**
 * debug function for displaying an answer
 * @param *answer pointer to a answer_t struct
 */
void _print_answer (struct answer_t * answer);
