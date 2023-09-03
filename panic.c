#include "panic.h"
#include <stdlib.h>
#include <string.h>

static panic_t paniccode = PANIC_NONE;

panic_t panic(panic_t code) {
	return paniccode = code;
}

panic_t panicked() {
	return paniccode;
}

const char  *panic_string() {
	switch(paniccode) {
		case PANIC_NONE: 
			return "Fine"; break;
		case PANIC_MEMORY:
			return "Could not manage memory correctly, sorry"; break;
		case PANIC_WMLIST_CONCATENATE:
			return "attempt_add(): could not build path string"; break;
		case PANIC_WMLIST_INVALID_SEARCH_DIR:
			return "wml_enumerate(): invalid search directory for available window managers"; break;
		case PANIC_PAM_START: 
			return "pam_start(): could not start PAM"; break;
		case PANIC_PAM_AUTHENTICATION:
			return "pam_authenticate(): could not authenticate user"; break;
		case PANIC_PAM_VERIFY:
			return "pam_acct_mgmt(): could not verify or allow user at this time"; break;
		case PANIC_PAM_CREDENTIALS:
			return "pam_setcred(): could not establish credentials"; break;
		case PANIC_PAM_CREDENTIALS_DELETE:
			return "pam_setcred(): could remove credentials properly"; break;
		case PANIC_PAM_SESSION_CLOSE:
			return "pam_close_session(): could not close session properly"; break;
		case PANIC_PAM_SESSION:
			return "pam_open_session(): could not open a session for the user"; break;
		case PANIC_MAX:  
			return "Max???"; break;
		default:
			return "Super panic!"; break;
	}
	return ""; /* Unreachable */
}

void 		*panicdata[32];
unsigned int     panicdatalen = 0;

void panic_data_push(void *data, unsigned int size) {
	if(panicdatalen >= 32)
		return;
	panicdata[panicdatalen] = malloc(size);
	memcpy(panicdata[panicdatalen], data, size);
	panicdatalen++;
}

unsigned int panic_data_len() {
	return panicdatalen;
}

void *panic_data_get(unsigned int i) {
	if(i >= panicdatalen)
		return NULL;
	return panicdata[i];
}

void panic_data_flush() {
	unsigned int i;
	for(i = 0; i < panicdatalen; i++)
		free(panicdata[i]);
	panicdatalen = 0;
}




