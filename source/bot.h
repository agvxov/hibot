#ifndef BOT_H

#include <libircclient.h>

#include <string.h>
#include <ctype.h>

extern syntax_setter_t syntax_functions[];

irc_session_t * session;
irc_callbacks_t callbacks;

void ircmsg(const char * const message) {
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

// XXX: msg ChanServ IDENTIFY?
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
		for (char * s = message + 1; *s != '\0'; s++) {
			*s = toupper(*s);
		}
		int l = translate_language(message + 1);
		message = terminator + 1;
		if (l != -1) {
			language = l;
			syntax_functions[language]();
		}
	}

	if (is_code) {
		char * buffer = (char *)origin;
		while (*(buffer++) != '!') { ; }
		asprintf(&buffer, "From %.*s:", (int)(buffer - origin)-1, origin);
		ircmsg(buffer);
		free(buffer);

		ircmsg(syntax_highlight(message));

		ircmsg("--");
	}

	free(message_guard);
}

int connect_bot(const char * const server, const short port) {
	memset(&callbacks, 0, sizeof(callbacks));
	callbacks.event_connect = event_connect;
	callbacks.event_privmsg = event_privmsg;
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
