#ifndef AIQBD_SOCKET_DEFAULTS
#define AIQBD_SOCKET_DEFAULTS

#define AIQBD_SOCKET_MAX_BUFFER 256
#define AIQBD_SOCKET_MAX_BACKLOG 16

#endif

enum aiqbd_ct {
	aiqbd_socket_interface
	};

unsigned long _aiqbd_register_problem (struct problem_t * problem);
unsigned long _aiqbd_register_answer (struct answer_t * answer);

void * _aiqbd_handling_socket_client (void * arg);
void * _aiqbd_handling_socket (void * arg);
void * _aiqbd_handling_problem (void * arg);
void * _aiqbd_handling_answer (void * arg);
