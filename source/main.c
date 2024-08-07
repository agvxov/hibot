#define PROGRAM_NAME "hibot"
#define _GNU_SOURCE

#include "config.inc"

char * channel;
char * server;
char * port;

typedef enum {
	C,
	CPP,
	ASM,
	ADA,
} language_t;

typedef void (*syntax_setter_t)(void);

#include "log.h"
#include "syntax.h"
#include "request.h"
#include "bot.h"

syntax_setter_t syntax_functions[] = {
	[C]   = &syntax_c,
	[CPP] = &syntax_cpp,
	[ASM] = &syntax_fasm,
	[ADA] = &syntax_ada,
};


const char help_message[] =
	PROGRAM_NAME " <server>:<port> <channel>"
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

	log_file = LOG_FILE;

	syntax_functions[DEFAULT_LANGUAGE]();

	connect_bot(server, port_i);
	connection_loop();

	return 0;

	USAGE_ERROR:
	puts(help_message);
	return 1;
}
