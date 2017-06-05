#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "pack.h"

int main (int argc, char ** argv) {
	double data[8] = { 1.00, 2.00, 2.01, 1.32, 2.33, 43.20, 22.10, 22.0 };
	unsigned long test[2] = { 1, 2 };
	unsigned long cv[2] = { 3, 4 };
	double state[6] = { 2.00, 3.00, 4.00, 1.00, 2.00, 6.00 };

	struct problem_t problem;
	struct file_header_t header;

	unsigned long length;

	problem.type = 1;
	problem.optm = 2;

	problem.rows = 4;
	problem.size_x = 1;
	problem.size_y = 1;

	problem.cv_size = 2;
	problem.test_size = 2;
	problem.state_size = 6;

	problem.cv_data = cv;
	problem.test_data = test;
	problem.data = data;
	problem.state = state;


	header.id = 1;
	header.parent = 0;
	header.priority = 0;
	header.status = 'N';

	length = _problem_pack (argv[1], &problem, &header);


	printf ("%ld bytes\n", length);
	}
