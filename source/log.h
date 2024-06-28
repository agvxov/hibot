#ifndef LOG_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

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

void logf_notice(const char * const format, ...) {
    va_list args;
    va_start(args, format);

    char * msg;
    vasprintf(&msg, format, args);
    log_notice(msg);
    free(msg);
}

void log_error(const char * const message) {
	log("\033[33m", message);
}

void logf_error(const char * const format, ...) {
    va_list args;
    va_start(args, format);

    char * msg;
    vasprintf(&msg, format, args);
    log_error(msg);
    free(msg);
}

#define LOG_H
#endif
