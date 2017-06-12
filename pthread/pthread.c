#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void * _communicate_thread (void * arg) {
	}

void * _train_thread (void * arg) {
	}

void * _solve_thread (void * arg) {
	}

void * print_message_function (void * ptr) {
	char * message;
	message = (char *) ptr;
	printf ("%s\n", message);
	}

int main (int argc, char ** argv) {
	pthread_t thread_a, thread_b;
	const char * message_a = "Thread A";
	const char * message_b = "Thread B";
	int iret_a, iret_b;

	iret_a = pthread_create (&thread_a, NULL, print_message_function, (void *) message_a);
	if (iret_a) {
		fprintf (stderr, "Error %d\n", iret_a);
		return 1;
		}
	iret_a = pthread_detach (thread_a);
	if (iret_a) {
		fprintf (stderr, "Error %d\n", iret_a);
		return 1;
		}

	iret_b = pthread_create (&thread_b, NULL, print_message_function, (void *) message_b);
	if (iret_b) {
		fprintf (stderr, "Error %d\n", iret_b);
		return 1;
		}

	pthread_join (thread_b, NULL);
	return 0;
	}
