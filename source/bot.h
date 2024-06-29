#ifndef BOT_H

#include <libircclient.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <unistd.h>

extern syntax_setter_t syntax_functions[];

static irc_session_t * session;
static irc_callbacks_t callbacks;

static inline
void irc_message(const char * const message) {
    log_notice(message);
	irc_cmd_msg(session, channel, message);
}

static inline
void irc_private_message(const char * const user, const char * const message) {
	irc_cmd_msg(session, user, message);
}

static inline
char * username_root(const char * const fullname){
	char * r = (char *)fullname;
	while (*(r++) != '!') { ; }
	asprintf(&r, "From %.*s:", (int)(r - fullname)-1, fullname);
	return r;
}

void on_request_timeout(int unused) {
    (void)(unused);
    logf_notice("Message time out: %p (%d)", (void*)request_queue[0], request_queue_head);
    drop_request(request_queue[0]);
}

void flush_request(request_t * request) {
	// Message header
	char * short_name = username_root(request->user);
	irc_message(short_name);
	free(short_name);

    // Message body
    syntax_count = 0;
    syntax_functions[request->language]();
    for (unsigned i = 0; i < request->buffer_head; i++) {
        irc_message(syntax_highlight(request->buffer[i]));
    }

	// Message footer
	irc_message("--");

	logf_notice("Flushed message: %p (%d)", (void*)request, request_queue_head);

    drop_request(request);
}

static
language_t translate_language(const char * const language) {
	if (!strcmp(language, "C")) {
		return C;
	} else if (!strcmp(language, "C++") || !strcmp(language, "CPP")) {
		return CPP;
	} else if (!strcmp(language, "ASM") || !strcmp(language, "FASM") || !strcmp(language, "ASSEMBLY")) {
		return ASM;
	} else if (!strcmp(language, "ADA")) {
		return ADA;
	}
	return -1;
}

static
void irc_help() {
	irc_message(PROGRAM_NAME " "
#include "version.inc"
				);
	irc_message(PROGRAM_NAME " is a code highlighting IRC bot."
					" You may direct message it with your code or commands."
				);
	irc_message("Syntax:");
	irc_message("  !help               // print help");
	irc_message("  !<language>         // set language for next message");
	irc_message("  <code>              // echo this code");
	irc_message("  !<language> <code>  // set language and echo code");
	irc_message("  !--                 // flush all code");
	irc_message("--");
}

// XXX: msg ChanServ IDENTIFY?
static
void event_connect(irc_session_t * session,
							const char	* event,
							const char	* origin,
							const char ** params,
							unsigned int count) {
	(void)event;
	(void)origin;
	(void)params;
	(void)count;

	log_notice("IRC connection secured.");
	irc_cmd_join(session, channel, 0);
	logf_notice("Joined destination channel: `%s`.", channel);
}


// XXX: event_disconnect() is not a real function,
//       use int irc_is_connected(irc_session_t * session) instead

static
void event_privmsg(irc_session_t * session,
                   const char  * event,
                   const char  * origin,
                   const char ** params,
                   unsigned int count) {
	(void)session;
	(void)event;
	(void)count;

	char * const message_guard = strdup(params[1]);
	char *       message       = message_guard;
	char * terminator;
	int is_code = 1;

	/* Is command */
	if (*message == '!') {
		if (!strcmp(message, "!help")) {
			irc_help();
			goto END;
		}
		/* */
		terminator = message;
		while (*terminator != ' ') {
			if (*terminator == '\0') {
				is_code = 0;
				break;
			}
			++terminator;
		}
		*terminator = '\0';
        /* */
        {
            request_t * request = take_request(origin);

            if (!strcmp(message, "!--")) {
                if (request) {
                    flush_request(request);
                }
                goto END;
            }

            /* get language */
            for (char * s = message + 1; *s != '\0'; s++) {
                *s = toupper(*s);
            }
            int l = translate_language(message + 1);
            message = terminator + 1;
            if (l != -1) {
                request->language = l;
            }
        }
	}

	/* Is code */
	if (is_code) {
		request_t * request = take_request(origin);
		if (!request) {
			irc_private_message(origin, message_queue_full_message);
			goto END;
		}

		touch_request_timer(request);
		request->buffer[request->buffer_head++] = strdup(message);
	}

	END:
	free(message_guard);
}

static
void event_channel(irc_session_t * session,
                   const char  * event,
                   const char  * origin,
                   const char ** params,
                   unsigned int count) {
	(void) session;
	(void) event;
	(void) origin;
	(void) count;

	const char * const message = params[1];

	if (!strncmp(message, "!help", sizeof("!help")-1)) {
		irc_help();
	}
}


int connect_bot(const char * const server, const short port) {
	memset(&callbacks, 0, sizeof(callbacks));
	callbacks.event_connect = event_connect;
	callbacks.event_privmsg = event_privmsg;
	callbacks.event_channel = event_channel;
	session = irc_create_session(&callbacks);

	if (!session) {
		log_error("Error creating IRC session.");
		return 1;
	} else {
		log_notice("IRC Session initialized.");
	}

	for (unsigned int i = 0; i < message_queue_size; i++) {
		request_queue[i] = &request_queue__[i];
		init_request(request_queue[i]);
	}
	signal(SIGALRM, on_request_timeout);

	irc_connect(session,
	            server,
	            port,
	            password,
	            username,
	            username,
	            username
	);

	return 0;
}

int connection_loop(void) {
	if (irc_run(session) != 0) {
		log_error("Error running IRC session\n"
			"Possible issue: bad URL, no network connection, bad port, refused connection.");
	}
	return 0;
}

#define BOT_H
#endif
