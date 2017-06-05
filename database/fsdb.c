#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <pthread.h>

#include "../pack/pack.h"
#include "fsdb.h"

/**
 * function, checking if dir_name is dir
 * @param *dir_name, the directory name
 * @return 1 if dir_name is a searchable path
 */
unsigned short int _is_dir (char * dir_name) {
	DIR * dir;
	if (dir = opendir (dir_name)) {
		closedir (dir);
		return 1;
		}
	return 0;
	}

/**
 * concatenate function for creating a path
 * @param *prefix, the directory
 * @param *suffix, the file
 * @return * pointer to the name of the full path
 * !require free result
 */
char * _concat_path (char * prefix, char * suffix) {
	int prefix_len, suffix_len, concat_len, c;
	char * concat;

	prefix_len = strlen (prefix);
	while ((*(prefix + prefix_len - 1) == DIRECTORY_SEPARATOR) && prefix_len > 0) prefix_len --;
	suffix_len = strlen (suffix);

	concat_len = prefix_len + suffix_len + 2;

	if (prefix_len == 0)
		concat_len ++;

	concat = (char *) malloc (concat_len * sizeof (char));
	if (!concat) return NULL;

	if (prefix_len > 0) {
		memcpy (concat, prefix, prefix_len);
		*(concat + prefix_len) = DIRECTORY_SEPARATOR;
		}
	else {
		*(concat + prefix_len) = '.';
		*(concat + ++prefix_len) = DIRECTORY_SEPARATOR;
		}
	memcpy (concat + prefix_len + 1, suffix, suffix_len);
	*(concat + prefix_len + suffix_len + 1) = 0;

	return concat;
	};

/**
 * loads file database into fsdb_t struct
 * @param *dir_name, the name of the directory
 * @return a pointer to a struct fsdb_t
 * @see fsdb_t
 */
struct fsdb_t * _load_db (char * dir_name) {
	DIR * dir;
	struct dirent * ent;
	struct fsdb_t * atom;
	struct fsdb_t * db = NULL;

	if ((dir = opendir (dir_name)) == NULL) return;

	while ((ent = readdir (dir)) != NULL) {
		if (*(ent->d_name) == '.')
			continue;

		atom = (struct fsdb_t *) malloc (sizeof (struct fsdb_t));
		if (!atom) {
			closedir (dir);
			_free_db (db);
			return NULL;
			}
		atom->name = _concat_path (dir_name, ent->d_name);
		if (_is_dir (atom->name)) {
			free (atom->name);
			free (atom);
			continue;
			}

		atom->header = (struct file_header_t *) malloc (sizeof (struct file_header_t));
		if (!atom->header) {
			closedir (dir);
			_free_db (db);
			return NULL;
			}
		if (_problem_read_metadata (atom->name, atom->header) == 0) {
			free (atom->name);
			free (atom->header);
			free (atom);
			continue;
			}

		if (db == NULL) {
			atom->prev = atom;
			atom->next = atom;
			db = atom;
			}
		else {
			atom->next = db;
			atom->prev = db->prev;
			db->prev->next = atom;
			db->prev = atom;
			}
		}
	
	closedir (dir);

	return db;
	}
/**
 * insert an element into fsdb_t struct
 * @param **db, pointer to database structure
 */
void _insert_db (struct fsdb_t ** db, struct fsdb_t * atom) {
	if (atom == NULL)
		return;
	if (*db == NULL) {
		atom->prev = atom;
		atom->next = atom;
		*db = atom;
		}
	else {
		atom->next = (*db);
		atom->prev = (*db)->prev;
		(*db)->prev->next = atom;
		(*db)->prev = atom;
		}
	}

/**
 * find an element from fsdb_t struct
 * @param *db, pointer to database structure
 * @param search structure (union)
 */
struct fsdb_t * _find_db (struct fsdb_t * db, struct query_t * query) {
	unsigned short int found = 0;
	struct fsdb_t * atom = db;

	if (db == NULL)
		return NULL;
	if (query == NULL)
		return db;

	do {
		if (query->type == query_by_id && query->query.id == atom->header->id) {
			found = 1;
			break;
			}
		if (query->type == query_by_parent && query->query.parent == atom->header->parent) {
			found = 1;
			break;
			}
		if (query->type == query_by_priority && query->query.priority == atom->header->priority) {
			found = 1;
			break;
			}
		if (query->type == query_by_status && query->query.status == atom->header->status) {
			found = 1;
			break;
			}
		atom = atom->next;
		}
	while (atom != db);
	if (!found)
		return NULL;
	return atom;
	}

/**
 * extract an element from fsdb_t struct
 * @param **db, pointer to database structure
 * @param search structure (union)
 * @return pointer to database atom
 */
struct fsdb_t * _extract_db (struct fsdb_t ** db, struct query_t * query) {
	struct fsdb_t * atom;

	if ((atom = _find_db (*db, query)) == NULL)
		return NULL;
		
	atom->prev->next = atom->next;
	atom->next->prev = atom->prev;

	if (*db == atom)
		*db = atom == atom->next ? NULL : atom->next;

	return atom;
	}

/**
 * count the number of elements from fsdb
 * @param *db, pointer to database structure
 * @return number of elements from database
 */
unsigned long int _count_db (struct fsdb_t * db) {
	unsigned long int count = 0;
	struct fsdb_t * atom = db;

	if (atom == NULL)
		return 0;

	do {
		count ++;
		atom = atom->next;
		}
	while (atom != db);

	return count;
	}

/**
 * get last inserted id from fsdb database
 * @param *db, pointer to database structure
 * @return the last id inserted (max of ids)
 */
unsigned long int _last_id_db (struct fsdb_t * db) {
	unsigned long last_id = 0;
	struct fsdb_t * atom = db;

	if (db == NULL)
		return last_id;

	do {
		last_id = last_id > atom->header->id ? last_id : atom->header->id;
		atom = atom->next;
		}
	while (atom != db);

	return last_id;
	}

/**
 * frees the memory for only one atom
 * @param *atom, pointer to database atom
 */
void _free_atom (struct fsdb_t * atom) {
	if (atom == NULL)
		return;
	free (atom);
	}

/**
 * closes the database from fsdb_t struct
 * @param *db, pointer to database structure
 */
void _free_db (struct fsdb_t * db) {
	struct fsdb_t * deleted, * atom;
	if (db == NULL)
		return;

	atom = db->next;
	while (atom != NULL) {
		deleted = atom;
		atom->prev->next = NULL;
		atom = atom->next;
		_free_atom (deleted);
		}
	}

/**
 * display the database content from fsdb_t struct
 * @param *db, pointer to database structure
 */
void _print_db (struct fsdb_t * db) {
	struct fsdb_t * atom = db;

	if (atom == NULL) {
		printf ("void\n");
		return;
		}

	do {
		printf ("problem\n\tname: %s\n\tid: %ld\n\tpriority: %d\n\tstatus:%c\n", atom->name, atom->header->id, atom->header->priority, atom->header->status);
		atom = atom->next;
		}
	while (atom != db);
	}
