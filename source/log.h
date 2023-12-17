#ifndef LOG_H

#include <stdio.h>

FILE * log_file;

static
void log(const char * const message, const char * const color) {
	fputs(color, log_file);
	fputs(message, log_file);
	fputs("\033[0m\n", log_file);
	fflush(log_file);
}

void log_notice(const char * const message) {
	log("", message);
}

void log_error(const char * const message) {
	log("\033[33m", message);
}

#define LOG_H
#endif
