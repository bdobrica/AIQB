#ifndef DAEMON_NAME
#define DAEMON_NAME "aiqbdaemon"
#endif

struct config_t _config;

void _shutdown (void);
void _signals (int sig);
void _daemonize (char *rundir, char *pidfile);
