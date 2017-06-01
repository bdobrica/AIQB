#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#ifndef SOCKET_MAX_BUFFER
#define SOCKET_MAX_BUFFER 256
#endif

#ifndef SOCKET_MAX_BACKLOG
#define SOCKET_MAX_BACKLOG 16
#endif

void _socket_error (const char * msg) {
	printf ("%s\n", msg);
	exit (1);
	}

void _socket_client (int socket_fd) {
	int new_socket_fd, n;
	char buffer[SOCKET_MAX_BUFFER];
	socklen_t client_len;
	struct sockaddr_in client_addr;

	if ((new_socket_fd = accept (socket_fd, (struct sockaddr *) &client_addr, &client_len)) < 0) return;
	
	bzero (buffer, sizeof (buffer));
	n = read (new_socket_fd, buffer, sizeof (buffer) - 1);
	if (n < 0) {
		close (new_socket_fd);
		return;
		}

	printf ("message: %s\n", buffer);

	close (new_socket_fd);
	}

int main (int argc, char ** argv) {
	int socket_fd, new_socket_fd, port_no;
	socklen_t client_len;
	char buffer[SOCKET_MAX_BUFFER];
	struct sockaddr_in server_addr, client_addr;
	int n;

	socket_fd = socket (AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0) {
		_socket_error ("error opening socket");
		}

	bzero ((char *) &server_addr, sizeof (server_addr));
	port_no = atoi (argv[1]);

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons (port_no);

	if (bind (socket_fd, (struct sockaddr *) &server_addr, sizeof (server_addr)) < 0) {
		_socket_error ("error binding");
		}
	listen (socket_fd, SOCKET_MAX_BACKLOG);

	fcntl (socket_fd, F_SETFL, O_NONBLOCK);

	while (1) {
		_socket_client (socket_fd);
		}

	close (socket_fd);
	return 0;
	}
