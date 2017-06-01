#include <stdio.h>
#include <signal.h>
#include <syslog.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#include "../pack/pack.h"
#include "../database/fsdb.h"
#include "../config/config.h"

#include "daemon.h"

void _signals (int sig) {
	switch (sig) {
		case SIGHUP:
			syslog (LOG_WARNING, "Received SIGHUP signal.");
			break;
		case SIGINT:
		case SIGTERM:
			syslog (LOG_INFO, "Daemon exiting");
			_shutdown ();
			exit (EXIT_SUCCESS);
			break;
		default:
			syslog (LOG_WARNING, "Unhandled signal %s", strsignal (sig));
			break;
		}
	}

void _shutdown () {
	_free_config (&_config);
	}

void _daemonize (char * rundir, char * pidfile) {
	int pid, sid, i;
	char str[10];
	struct sigaction _sig_action;
	sigset_t _sig_set;

	/** check if parent process id is set */
	if (getppid () == 1) {
		/** PPID exists, therefore we are already a daemon */
		return;
		}

	/** set signal mask - signals we want to block */
	sigemptyset (&_sig_set);
	sigaddset (&_sig_set, SIGCHLD);  /** ignore child - i.e. we don't need to wait for it */
	sigaddset (&_sig_set, SIGTSTP);  /** ignore tty stop signals */
	sigaddset (&_sig_set, SIGTTOU);  /** ignore tty background writes */
	sigaddset (&_sig_set, SIGTTIN);  /** ignore tty background reads */
	sigprocmask (SIG_BLOCK, &_sig_set, NULL);   /** block the above specified signals */

	/** set up a signal handler */
	_sig_action.sa_handler = _signals;
	sigemptyset (&_sig_action.sa_mask);
	_sig_action.sa_flags = 0;

	/** signals to handle */
	sigaction (SIGHUP, &_sig_action, NULL);     /** catch hangup signal */
	sigaction (SIGTERM, &_sig_action, NULL);    /** catch term signal */
	sigaction (SIGINT, &_sig_action, NULL);     /** catch interrupt signal */

	/** fork*/
	pid = fork ();

	if (pid < 0) {
		/** could not fork */
		exit (EXIT_FAILURE);
		}

	if (pid > 0) {
		/** child created ok, so exit parent process */
		printf ("Child process created: %d\n", pid);
		exit (EXIT_SUCCESS);
		}

	/** child continues */
	umask (027); /** set file permissions 750 */

	/** get a new process group */
	sid = setsid ();

	if (sid < 0) {
		printf ("Could not start new session.\n");
		exit (EXIT_FAILURE);
		}

	/** close all descriptors */
	for (i = getdtablesize (); i >= 0; --i) {
		close (i);
		}

	/** route I/O connections */
	/** open STDIN */
	i = open ("/dev/null", O_RDWR);

	/** STDOUT */
	dup (i);

	/** STDERR */
	dup (i);

	chdir (rundir); /** change running directory */

	/** ensure only one copy */
	_config.pid_file_d = open (pidfile, O_RDWR | O_CREAT, 0600);

	if (_config.pid_file_d == -1) {
		/** couldn't open lock file */
		syslog (LOG_INFO, "Could not open PID lock file %s, exiting", pidfile);
		exit (EXIT_FAILURE);
		}

	/** try to lock file */
	if (lockf (_config.pid_file_d, F_TLOCK, 0) == -1) {
		/** couldn't get lock on lock file */
		syslog (LOG_INFO, "Could not lock PID lock file %s, exiting", pidfile);
		exit (EXIT_FAILURE);
		}

	/** get and format PID */
	sprintf (str, "%d\n", getpid());

	/** write pid to lockfile */
	write (_config.pid_file_d, str, strlen(str));
	}

void _daemon_solve (char (*solving_function)(struct problem_t * problem), struct fsdb_t * atom) {
	struct problem_t * problem;
	unsigned long length;
	char status;

	problem = (struct problem_t *) malloc (sizeof (struct problem_t));
	if (!problem) {
		return;
		}

	length = _problem_unpack (atom->name, problem);
	status = solving_function (problem);
	length = _problem_pack (atom->name, problem);
	}

void _daemon_query (double (*query_function)(struct problem_t * problem), struct fsdb_t * atom) {
	struct problem_t * problem;
	unsigned long length;
	char status;

	problem = (struct problem_t *) malloc (sizeof (struct problem_t));
	if (!problem) {
		return;
		}
	
	length = _problem_unpack (atom->name, problem);
	query_function (problem);
	}

void _daemon_config (int argc, char ** argv) {
	}
