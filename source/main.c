#define PROGRAM_NAME "hibot"
#define _GNU_SOURCE

#include "config.inc"

char * channel;
char * server;
char * port;

#include <stdio.h>
FILE * log_file;

#include "log.h"
//#include "syntax.h"
#include "bot.h"

const char help_message[] =
	PROGRAM_NAME " <server>:<port> <channel>\n"
;

signed main(int argc, char * * argv) {
	if (argc != 3) {
		goto USAGE_ERROR;
	}

	channel = argv[2];
	server  = strdup(argv[1]);
	port    = strchr(server, ':');
	if (port) {
		*port = '\0';
	} else {
		goto USAGE_ERROR;
	}
	++port;
	int port_i = 0;
	if(!sscanf(port, "%hd", &port_i)) {
		goto USAGE_ERROR;
	}

	log_file = stdout;

	connect_bot(server, port_i);
	connection_loop();

	return 0;

	USAGE_ERROR:
	puts(help_message);
	return 1;
}
