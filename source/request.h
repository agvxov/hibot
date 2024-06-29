#ifndef REQUEST_H
#define REQUEST_H

#include <stdlib.h>
#include <sys/time.h>

typedef struct {
	char * user;
	language_t language;
	struct itimerval timer;
	char * buffer[128];			// XXX: no overflow detection/avertion
	unsigned int buffer_head;   //       is implemented on this bunch
} request_t;

request_t   request_queue__[message_queue_size];
request_t * request_queue[message_queue_size];
unsigned int request_queue_head = 0;

static inline
void init_request(request_t * request) {
	request->timer.it_value.tv_sec     = 0;
	request->timer.it_value.tv_usec    = 0;
	request->timer.it_interval.tv_sec  = 0;
	request->timer.it_interval.tv_usec = 0;
    request->language = DEFAULT_LANGUAGE;
	request->buffer_head = 0;
}

static inline
void reinit_request(request_t * request) {
	free(request->user);
    for (long i = 0; i < request->buffer_head; i++) {
        free(request->buffer[i]);
    }
    init_request(request);
}

static inline
void touch_request_timer(request_t * const request) {
    struct itimerval v;
	request->timer.it_value.tv_sec = message_timeout;
	setitimer(ITIMER_REAL, &(request->timer), &v);
    logf_notice("Timer touched (old value: %ds)", v.it_value.tv_sec);
}

request_t * take_request(const char * const user) {
	for (unsigned int i = 0; i < request_queue_head; i++) {
		if(!strcmp(request_queue[i]->user, user)) {
			return request_queue[i];
		}
	}

	if (request_queue_head == message_queue_size) {
		return NULL;
	}

	request_t * request = request_queue[request_queue_head];

	request->user = strdup(user);

	++request_queue_head;

	logf_notice("Took message: %p (%d)", (void*)request, request_queue_head);

	return request;
}

void drop_request(request_t * const request) {
	setitimer(ITIMER_REAL, NULL, NULL);

    reinit_request(request);

	if (message_queue_size > 1) {
		for (unsigned int i = 0; i < request_queue_head; i++) {
			request_queue[i] = request_queue[i+1];
		}
		request_queue[request_queue_head] = request;
	}
	
	--request_queue_head;

	logf_notice("Dropped message: %p (%d)", (void*)request, request_queue_head);
}

#endif
