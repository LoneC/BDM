#ifndef _BDM_AUTH_H_
#define _BDM_AUTH_H_

#include <sys/types.h>

int authenticate(const char *uname, const char *pword);
int login();
int logout();


#endif
