#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "pack.h"

int main (int argc, char ** argv) {
	struct problem_t problem;
	int c;
	unsigned long length;

	/*
	problem.cv_data = cv; 2
	problem.test_data = test; 2
	problem.data = data; 8
	problem.state = state; 6
	*/

	length = _problem_unpack (argv[1], &problem);
	printf ("%ld bytes\n", length);

	for (c = 0; c < 8; c++) {
		printf ("data[%d] = %lf\n", c, *(problem.data + c));
		}
	for (c = 0; c < 6; c++) {
		printf ("state[%d] = %lf\n", c, *(problem.state + c));
		}
	for (c = 0; c < 2; c++) {
		printf ("cv_data[%d] = %ld\n", c, *(problem.cv_data + c));
		}
	for (c = 0; c < 2; c++) {
		printf ("test_data[%d] = %ld\n", c, *(problem.test_data + c));
		}
	}
