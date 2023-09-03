#include "userstring.h"

#include "panic.h"
#include <stdlib.h>

const uint64_t usrstrbuffermore = 32;

void usrstr_init(usrstr *is) {
	is->buffer = malloc(usrstrbuffermore);
	is->buffer[0] = '\0';
	is->max = usrstrbuffermore;
	is->len = 0;
}

void usrstr_free(usrstr *is) {
	free(is->buffer);
	is->max = 0;
	is->len = 0;
}

void usrstr_push(usrstr *is, char c) {
	is->buffer[is->len] = c;
	is->len++;
	if(is->len >= is->max) {
		is->buffer = realloc(is->buffer, is->max + usrstrbuffermore);
		is->max += usrstrbuffermore;
	}
	is->buffer[is->len] = '\0';
}

void usrstr_pop(usrstr *is) {
	if(is->len == 0)
		return;
	is->len--;
	is->buffer[is->len] = '\0';
}
