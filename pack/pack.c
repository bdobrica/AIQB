#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>

#include "pack.h"

/**
 * function for packing an IA problem in a file
 * @param *name the name of the file
 * @param *problem pointer to the problem datastructure
 * @return length of the written file
 */
unsigned long _problem_pack (unsigned char * name, struct problem_t * problem, struct file_header_t * file_header) {
	union _file_header metadata;
	union _problem_header header;
	union _problem_atom atom;
	unsigned long size, c, length;
	FILE * file;

	header.problem.type = (uint16_t) problem->type;
	header.problem.optm = (uint16_t) problem->optm;

	header.problem.rows = (uint64_t) problem->rows;
	header.problem.size_x = (uint32_t) problem->size_x;
	header.problem.size_y = (uint32_t) problem->size_y;

	header.problem.cv_size = (uint64_t) problem->cv_size;
	header.problem.test_size = (uint64_t) problem->test_size;
	header.problem.state_size = (uint64_t) problem->state_size;

	size = problem->rows * (problem->size_x + problem->size_y);

	if (header.problem.rows == 0 || header.problem.size_x == 0 || problem->data == NULL) return 0;

	file = fopen (name, "wb");
	if (!file) return 0;

	memcpy (metadata.header.prefix, "AIQB", 4);

	if (file_header != NULL) {
		metadata.header.id = file_header->id;
		metadata.header.parent = file_header->parent;
		metadata.header.priority = file_header->priority;
		metadata.header.time = (unsigned long) time (NULL);
		metadata.header.status = file_header->status;

		printf ("file metadata:\nprefix=%.4s\nid=%ld\nparent=%ld\npriority=%d\ntime=%ld\nstatus=%c\n",
			metadata.header.prefix,
			file_header->id,
			file_header->parent,
			file_header->priority,
			metadata.header.time,
			file_header->status);
		}
	else {
		metadata.header.id = 0;
		metadata.header.parent = 0;
		metadata.header.priority = 0;
		metadata.header.time = (unsigned long) time (NULL);
		metadata.header.status = 0;
		}

	fwrite (metadata.bin, sizeof (uint8_t), sizeof (metadata.bin), file);

	fwrite (header.bin, sizeof (uint8_t), sizeof (header.bin), file);

	length = sizeof (header.bin);
	if (problem->cv_size) {
		for (c = 0; c < problem->cv_size; c++) {
			atom.index = * (problem->cv_data + c);
			fwrite (atom.bin, sizeof (uint8_t), sizeof (atom.bin), file);
			length += sizeof (atom.bin);
			}
		}

	if (problem->test_size) {
		for (c = 0; c < problem->test_size; c++) {
			atom.index = * (problem->test_data + c);
			fwrite (atom.bin, sizeof (uint8_t), sizeof (atom.bin), file);
			length += sizeof (atom.bin);
			}
		}

	for (c = 0; c < size; c++) {
		atom.data = *(problem->data + c);
		fwrite (atom.bin, sizeof (uint8_t), sizeof (atom.bin), file);
		length += sizeof (atom.bin);
		}
	
	if (problem->state_size) {
		for (c = 0; c < problem->state_size; c++) {
			atom.data = *(problem->state + c);
			fwrite (atom.bin, sizeof (uint8_t), sizeof (atom.bin), file);
			length += sizeof (atom.bin);
			}
		}

	fclose (file);
	return length;
	}

/**
 * function for unpacking an AI problem from a file
 * @param *name the name of the file
 * @param *problem a pointer to a problem datastructure which will be populated with data
 * @return length of the read data
 */
unsigned long _problem_unpack (unsigned char * name, struct problem_t * problem) {
	union _file_header metadata;
	union _problem_header header;
	union _problem_atom atom;
	FILE * file;
	unsigned long size, c, read, length;

	file = fopen (name, "rb");
	if (!file) return 0;

	read = fread (metadata.bin, sizeof (uint8_t), sizeof (metadata.bin), file);
	if (read != sizeof (metadata.bin)) {
		fclose (file);
		return 0;
		}

	read = fread (header.bin, sizeof (uint8_t), sizeof (header.bin), file);
	if (read != sizeof (header.bin)) {
		fclose (file);
		return 0;
		}
	
	length += read;

	problem->type = (unsigned int) header.problem.type;
	problem->optm = (unsigned int) header.problem.optm;

	problem->rows = (unsigned long) header.problem.rows;
	problem->size_x = (unsigned int) header.problem.size_x;
	problem->size_y = (unsigned int) header.problem.size_y;

	problem->cv_size = (unsigned long) header.problem.cv_size;
	problem->test_size = (unsigned long) header.problem.test_size;
	problem->state_size = (unsigned long) header.problem.state_size;

	size = problem->rows * (problem->size_x + problem->size_y);

	problem->data = (double *) malloc (size * sizeof (double));
	if (!problem->data) {
		fclose (file);
		return 0;
		}

	if (problem->cv_size) {
		problem->cv_data = (unsigned long *) malloc (problem->cv_size * sizeof (unsigned long));
		if (!problem->cv_data) {
			fclose (file);
			free (problem->data);
			return 0;
			}
		}
	
	if (problem->test_size) {
		problem->test_data = (unsigned long *) malloc (problem->test_size * sizeof (unsigned long));
		if (!problem->test_data) {
			fclose (file);
			free (problem->data);
			free (problem->cv_data);
			return 0;
			}
		}

	if (problem->state_size) {
		problem->state = (double *) malloc (problem->state_size * sizeof (double));
		if (!problem->state) {
			fclose (file);
			free (problem->data);
			free (problem->cv_data);
			free (problem->test_data);
			return 0;
			}
		}
	
	for (c = 0; c < problem->cv_size; c++) {
		read = fread (atom.bin, sizeof (uint8_t), sizeof (atom.bin), file);
		*(problem->cv_data + c) = atom.index;
		length += read;
		if (read != sizeof (atom.bin))
			break;
		}
	if (c < problem->cv_size) {
		fclose (file);
		free (problem->cv_data);
		free (problem->test_data);
		free (problem->data);
		free (problem->state);
		return 0;
		}
	
	for (c = 0; c < problem->test_size; c++) {
		read = fread (atom.bin, sizeof (uint8_t), sizeof (atom.bin), file);
		*(problem->test_data + c) = atom.index;
		length += read;
		if (read != sizeof (atom.bin))
			break;
		}
	if (c < problem->cv_size) {
		fclose (file);
		free (problem->cv_data);
		free (problem->test_data);
		free (problem->data);
		free (problem->state);
		return 0;
		}

	for (c = 0; c < size; c++) {
		read = fread (atom.bin, sizeof (uint8_t), sizeof (atom.bin), file);
		*(problem->data + c) = atom.data;
		length += read;
		if (read != sizeof (atom.bin))
			break;
		}
	if (c < size) {
		fclose (file);
		free (problem->cv_data);
		free (problem->test_data);
		free (problem->data);
		free (problem->state);
		return 0;
		}

	for (c = 0; c < problem->state_size; c++) {
		read = fread (atom.bin, sizeof (uint8_t), sizeof (atom.bin), file);
		*(problem->state + c) = atom.data;
		length += read;
		if (read != sizeof (atom.bin))
			break;
		}
	if (c < problem->state_size) {
		fclose (file);
		free (problem->cv_data);
		free (problem->test_data);
		free (problem->data);
		free (problem->state);
		return 0;
		}
	
	return length - 1;
	fclose (file);
	}
/**
 * function for unpacking only the metadata of AI problem
 * @param *name the name of the file
 * @param *metadata the metadata structure
 * @return length of the read data
 */
unsigned short _problem_read_metadata (unsigned char * name, struct file_header_t * metadata) {
	union _file_header header;
	FILE * file;
	unsigned short read;

	file = fopen (name, "rb");
	if (!file) return 0;

	read = fread (header.bin, sizeof (uint8_t), sizeof (header.bin), file);
	if (read != sizeof (header.bin)) {
		fclose (file);
		return 0;
		}
	
	if (memcmp (header.header.prefix, "AIQB", 4) != 0) {
		fclose (file);
		return 0;
		}
	
	if (metadata != NULL) {
		memcpy (metadata->prefix, header.header.prefix, 4);
		metadata->id = header.header.id;
		metadata->parent = header.header.parent;
		metadata->priority = header.header.priority;
		metadata->time = header.header.time;
		metadata->status = header.header.status;
		}

	fclose (file);
	return read;
	}
/**
 * function for packing only the metadata of AI problem
 * @param *name the name of the file
 * @param *metadata the metadata structure
 * @return length of the written data
 */
unsigned short _problem_write_metadata (unsigned char * name, struct file_header_t * metadata) {
	union _file_header header;
	FILE * file;
	unsigned short write;

	file = fopen (name, "r+b");
	if (!file) return 0;

	memcpy (header.header.prefix, "AIQB", 4);
	header.header.id = metadata->id;
	header.header.parent = metadata->parent;
	header.header.priority = metadata->priority;
	header.header.time = (unsigned long) time (NULL);
	header.header.status = metadata->status;

	write = fwrite (header.bin, sizeof (uint8_t), sizeof (header.bin), file);
	if (write != sizeof (header.bin)) {
		fclose (file);
		return 0;
		}

	fclose (file);
	return write;
	}
/**
 * debug function for displaying a problem
 * @param *problem pointer to a problem_t struct
 */
void _print_problem (struct problem_t * problem) {
	unsigned long c, d;
	printf ("problem\n\ttype = %d\n\toptm = %d\n\n\trows = %ld\n\tsize_x = %ld\n\tsize_y = %ld\n\n\tcv_size = %ld\n\ttest_size = %ld\n\n\tstate_size = %ld\n",
		problem->type,
		problem->optm,
		problem->rows,
		problem->size_x,
		problem->size_y,
		problem->cv_size,
		problem->test_size,
		problem->state_size);
	printf ("\tcv_data = [  ");
	for (c = 0; c < problem->cv_size; c++)
		printf ("%ld, ", *(problem->cv_data + c));
	printf ("\b\b]\n\ttest_data = [  ");
	for (c = 0; c < problem->test_size; c++)
		printf ("%ld, ", *(problem->test_data + c));
	printf ("\b\b]\n\tdata = [  \n");
	for (c = 0; c < problem->rows; c++) {
		printf ("\t%ld:\t[  ", c + 1);
		for (d = 0; d < problem->size_x + problem->size_y; d++) {
			printf ("%.2lf, ", *(problem->data + c * (problem->size_x + problem->size_y) + d));
			}
		printf ("\b\b]\n");
		}
	printf ("\t]\n\tstate = [  ");
	for (c = 0; c < problem->state_size; c++) {
		printf ("%.2lf, ", *(problem->state + c));
		}
	printf ("\b\b]\n");
	}
/**
 *
 */
double * _problem_recover_X (struct problem_t * problem, int storage) {
	unsigned long features, examples, f, e;
	double * X, alpha = 1.00;

	examples = problem->rows;
	if ((storage & problem_add_bias) == problem_add_bias)
		features = problem->size_x + 1;
	else
		features = problem->size_x;
	
	X = (double *) malloc (examples * features * sizeof (double));
	if (X == NULL)
		return NULL;

	if ((storage & problem_row_major) == problem_row_major) {
		if ((storage & problem_add_bias) == problem_add_bias) {
			for (e = 0; e < examples; e++) {
				memcpy ((X + examples * e), &alpha, sizeof (double));
				memcpy ((X + examples * e + 1), (problem->data + (problem->size_x + problem->size_y) * e), problem->size_x * sizeof (double));
				}
			}
		else {
			for (e = 0; e < examples; e++) {
				memcpy ((X + examples * e), (problem->data + (problem->size_x + problem->size_y) * e), problem->size_x * sizeof (double));
				}
			}
		}
	
	if ((storage & problem_col_major) == problem_col_major) {
		if ((storage & problem_add_bias) == problem_add_bias) {
			for (e = 0; e < examples; e++) {
				for (f = 0; f < features; f++) {
					if (f == 0)
						*(X + f * examples + e) = alpha;
					else
						*(X + f * examples + e) = *(problem->data + e * (problem->size_x + problem->size_y) + (f - 1));
					}
				}
			}
		else {
			for (e = 0; e < examples; e++) {
				for (f = 0; f < features; f++) {
					*(X + f * examples + e) = *(problem->data + e * (problem->size_x + problem->size_y) + f);
					}
				}
			}
		}
	
	return X;
	}
/**
 *
 */
double * _problem_recover_y (struct problem_t * problem, int storage) {
	unsigned long features, examples, f, e;
	double * y, alpha = 1.00;

	features = problem->size_y;
	examples = problem->rows;

	y = (double *) malloc (examples * features * sizeof (double));
	if (y == NULL)
		return NULL;

	for (e = 0; e < examples; e ++) {
		for (f = 0; f < features; f++) {
			if ((storage & problem_row_major) == problem_row_major) {
				for (e = 0; e < examples; e++) {
					memcpy ((y + examples * e), (problem->data + (problem->size_x + problem->size_y) * e + problem->size_x), problem->size_y * sizeof (double));
					}
				}
			if ((storage & problem_col_major) == problem_col_major) {
				for (e = 0; e < examples; e++) {
					for (f = 0; f < features; f++) {
						*(y + f * examples + e) = *(problem->data + e * (problem->size_x + problem->size_y) + problem->size_x + f);
						}
					}
				}
			}
		}

	return y;
	}
/**
 *
 */
double * _problem_enhance_X (struct problem_t * problem, int enhance) {
	return NULL;
	}
