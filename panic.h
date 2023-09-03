#ifndef _BDM_PANIC_H_
#define _BDM_PANIC_H_

typedef enum _panic_e {
	PANIC_NONE = 0,
	PANIC_MEMORY,
	PANIC_WMLIST_CONCATENATE,
	PANIC_WMLIST_INVALID_SEARCH_DIR,
	PANIC_PAM_START,
	PANIC_PAM_AUTHENTICATION,
	PANIC_PAM_VERIFY,
	PANIC_PAM_CREDENTIALS,
	PANIC_PAM_CREDENTIALS_DELETE,
	PANIC_PAM_SESSION_CLOSE,
	PANIC_PAM_SESSION,
	PANIC_MAX
} panic_t;

panic_t      panic(panic_t code);
panic_t      panicked();
const char  *panic_string();

void         panic_data_push(void *data, unsigned int size);
unsigned int panic_data_len();
void        *panic_data_get(unsigned int i);
void         panic_data_flush();

#endif
