#ifndef CONFIG_DIR
#define CONFIG_DIR "/etc/aiqbd"
#endif

#ifndef CONFIG_FILE
#define CONFIG_FILE "config.ini"
#endif

#ifndef DATABASE_DIR
#define DATABASE_DIR "/var/lib/aiqbd"
#endif

#ifndef CONFIG_MAX_BUFFER
#define CONFIG_MAX_BUFFER 128
#endif

/**
 * @struct config_t
 * @var *config_dir, the configuration directory
 * @var *config_file, the configuration file
 * @var *database_dir, where the database is stored
 * @var *run_dir, where the daemon runs
 * @var *pid_file, the daemon pid file
 * @var *db, the database link
 * @var *queue, the queue link
 * @var no_daemon, tells if the daemon mode is not active
 * @var queue_size, the maximum number of elements in the queue
 * @var auto_increment, the next id to be assigned
 * @var port, the port where connections are expected
 * @var pid_file_d, the pid file descriptor
 */
struct config_t {
	char * config_dir;
	char * config_file;

	char * database_dir;

	char * run_dir;
	char * pid_file;

	struct fsdb_t * db;
	struct fsdb_t * queue;

	unsigned short int no_daemon;

	unsigned int queue_size;
	unsigned long auto_increment;

	int port;
	int pid_file_d;
	};

/**
 * trims chr from str string
 * @param chr the character needs to be trimmed
 * @param *str the string from which it needs to be trimmed
 */
void _trim (char chr, char * str);
/**
 * initialize the config structure with default values
 * @param *config the config structure
 * @return 1 on success
 * @see @struct config_t
 */
int _init_config (struct config_t * config);
/**
 * release the memory for config
 * @param *config the config structure
 */
void _free_config (struct config_t * config);
/**
 * assigns a value to a configuration parameter
 * @param *key, the name of the configuration parameter
 * @param *value, the value of the configuration parameter
 * @return 1 on success
 */
int _assign_config (char * key, char * value, struct config_t * config);
/**
 * parse command line passed args to config structure
 * @param argc argument count
 * @param argv argument values
 * @param *config _config structure pointer
 * @see @struct config_t
 */
void _parse_args (int argc, char ** argv, struct config_t * config);
/**
 * parse a configuration file. the name of the file is held in the
 * _config structure
 * @param *config _config structure pointer
 * @see @struct config_t
 */
void _parse_file (struct config_t * config);
