#ifndef BOT_H

#include <libircclient.h>

#include <string.h>
#include <ctype.h>

extern syntax_setter_t syntax_functions[];

static irc_session_t * session;
static irc_callbacks_t callbacks;

void irc_message(const char * const message) {
	irc_cmd_msg(session, channel, message);
}

static
language_t translate_language(const char * const language) {
	if (!strcmp(language, "C") || !strcmp(language, "C++")) {
		return C;
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
	char * buffer;
	asprintf(&buffer, "Joined destination channel: `%s`.", channel);
	log_notice(buffer);
	free(buffer);
}

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
		if (!strcmp(message, "!help")) {
			irc_help();
			goto END;
		}
		/* get language */
		for (char * s = message + 1; *s != '\0'; s++) {
			*s = toupper(*s);
		}
		int l = translate_language(message + 1);
		message = terminator + 1;
		if (l != -1) {
			language = l;
			syntax_count = 0;
			syntax_functions[language]();
		}
	}

	/* Is code */
	if (is_code) {
		char * buffer = (char *)origin;
		while (*(buffer++) != '!') { ; }
		asprintf(&buffer, "From %.*s:", (int)(buffer - origin)-1, origin);
		irc_message(buffer);
		free(buffer);

		irc_message(syntax_highlight(message));

		irc_message("--");
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
