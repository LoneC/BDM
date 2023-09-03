#ifndef _BDM_INPUT_STRING_
#define _BDM_INPUT_STRING_

#include <stdint.h>

typedef struct {
	char *buffer;
	uint64_t max;
	uint64_t len;
} usrstr;

extern const uint64_t usrstrbuffermore;

void usrstr_init(usrstr *us);
void usrstr_free(usrstr *us);
void usrstr_push(usrstr *us, char c);
void usrstr_pop(usrstr *us);

#endif
