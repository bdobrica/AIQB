#ifndef DIRECTORY_SEPARATOR
#define DIRECTORY_SEPARATOR '/'
#endif

/*
struct file_header_t {
	char prefix[4];
	unsigned long id;
	unsigned long parent;
	unsigned short int priority;
	unsigned long time;
	unsigned char status;
	};
*/
struct fsdb_t {
	char * name;
	struct file_header_t * header;

	pthread_t * handler;

	struct fsdb_t * prev;
	struct fsdb_t * next;
	struct fsdb_t * root;
	};

enum _query_type_ct {
	query_by_id,
	query_by_parent,
	query_by_priority,
	query_by_status
	};

union _query_t {
	unsigned long id;
	unsigned long parent;
	unsigned short int priority;
	unsigned char status;
	};

struct query_t {
	enum _query_type_ct type;
	union _query_t query;
	};

/**
 * function, checking if dir_name is dir
 * @param *dir_name, the directory name
 * @return 1 if dir_name is a searchable path
 */
unsigned short int _is_dir (char * dir_name);

/**
 * concatenate function for creating a path
 * @param *prefix, the directory
 * @param *suffix, the file
 * @return * pointer to the name of the full path
 * !require free result
 */
char * _concat_path (char * prefix, char * suffix);

/**
 * loads file database into fsdb_t struct
 * @param *dir_name, the name of the directory
 * @return a pointer to a struct fsdb_t
 * @see fsdb_t
 */
struct fsdb_t * _load_db (char * dir_name);

/**
 * insert an element into fsdb_t struct
 * @param *db, pointer to database structure
 */
void _insert_db (struct fsdb_t ** db, struct fsdb_t * atom);

/**
 * find an element from fsdb_t struct
 * @param *db, pointer to database structure
 * @param search structure (union)
 */
struct fsdb_t * _find_db (struct fsdb_t * db, struct query_t * query);

/**
 * extract an element from fsdb_t struct
 * @param *db, pointer to database structure
 * @param search structure (union)
 * @return pointer to database atom
 */
struct fsdb_t * _extract_db (struct fsdb_t ** db, struct query_t * query);

/**
 * count the number of elements from fsdb
 * @param *db, pointer to database structure
 * @return number of elements from database
 */
unsigned long int _count_db (struct fsdb_t * db);

/**
 * get last inserted id from fsdb database
 * @param *db, pointer to database structure
 * @return the last id inserted (max of ids)
 */
unsigned long int _last_id_db (struct fsdb_t * db);
/**
 * frees the memory for only one atom
 * @param *atom, pointer to database atom
 */

void _free_atom (struct fsdb_t * atom);

/**
 * closes the database from fsdb_t struct
 * @param *db, pointer to database structure
 */
void _free_db (struct fsdb_t * db);

/**
 * display the database content from fsdb_t struct
 * @param *db, pointer to database structure
 */
void _print_db (struct fsdb_t * db);
