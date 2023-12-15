#ifndef BOT_H

#include <libircclient.h>

#include <string.h>

irc_session_t * session;
irc_callbacks_t callbacks;

void ircmsg(const char * const message) {
	irc_cmd_msg(session, channel, message);
}

void event_connect(irc_session_t * session,
							const char	* event,
							const char	* origin,
							const char ** params,
							unsigned int count) {
	(void)event;
	(void)origin;
	(void)params;
	(void)count;
	/* msg ChanServ IDENTIFY? */
	log_notice("IRC connection secured.");
	irc_cmd_join(session, channel, 0);
	ircmsg("TEST");
}

void event_channel(irc_session_t * session,
							char const	* event,
							char const	* origin,
							char const ** params,
							unsigned int count) {
	(void)session;
	(void)event;
	(void)origin;
	(void)count;
	/* */
	char const * message = params[1];
	ircmsg(message);
}

int connect_bot(const char * const server, const short port) {
	memset(&callbacks, 0, sizeof(callbacks));
	callbacks.event_connect = event_connect;
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
