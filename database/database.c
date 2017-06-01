#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>

struct _config {
	char * db_server;
	char * db_user;
	char * db_pass;
	char * db;
	MYSQL * conn;
	};

void _init_config (struct _config * config) {
	config->db_server = "localhost";
	config->db_user = "root";
	config->db_pass = "stairwaytohaven";
	config->db = "gemini";
	config->conn = NULL;
	}

void _free_config (struct _config * config) {
	mysql_close (config->conn);
	}

int main (int argc, char ** argv) {
	struct _config config;
	MYSQL_RES * res;
	MYSQL_ROW row;

	_init_config (&config);

	config.conn = mysql_init (NULL);

	if (!mysql_real_connect (config.conn, config.db_server, config.db_user, config.db_pass, config.db, 0, NULL, 0)) {
		printf ("error connecting!\n");
		exit (1);
		}
	
	if (mysql_query (config.conn, "show tables")) {
		printf ("query error!\n");
		exit (1);
		}
	
	res = mysql_use_result (config.conn);
	printf ("result:\n");
	while ((row = mysql_fetch_row (res)) != NULL) {
		printf ("%s\n", *row);
		}
	mysql_free_result (res);
	
	_free_config (&config);
	}
