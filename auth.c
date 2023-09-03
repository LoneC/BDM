#include "panic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <sys/types.h>
#include <paths.h>
#include <pwd.h>

static const char *servicename = "BDM";
static const char *username = "";
static pam_handle_t *pamhandle = NULL;


static int pam_conv_cb(int num_msg, const struct pam_message **msg, struct pam_response **resp, void *appdata_ptr) {
	int i, result;
	
	if(*resp == NULL)
		return PAM_BUF_ERR;
	
	result = PAM_SUCCESS;
	for(i = 0; i < num_msg; i++) {
		char *uname, *pword;
		switch(msg[i]->msg_style) {
			case PAM_PROMPT_ECHO_ON:
				uname = ((char **)(appdata_ptr))[0];
				(*resp)[i].resp = strdup(uname);
				break;
			case PAM_PROMPT_ECHO_OFF:
				pword = ((char **)(appdata_ptr))[1];
				(*resp)[i].resp = strdup(pword);
				break;
			case PAM_ERROR_MSG:
				fprintf(stderr, "%s\n", msg[i]->msg);
				result = PAM_CONV_ERR;
				break;
			case PAM_TEXT_INFO:
				printf("%s\n", msg[i]->msg);
				break;
		}
		if(result != PAM_SUCCESS) {
			break;
		}
	}

	if(result != PAM_SUCCESS) {
		free(*resp);
		*resp = NULL;
		panic(PANIC_MAX + 1);
	}

	return result;
}

static void set_var(const char *var, const char *val) {
	uint64_t len = strlen(var) + strlen(val) + 2;
	char *buffer = malloc(len);

	/* Concatenate */
	uint64_t i, last;
	for(i = 0; var[i]; i++)
		buffer[i] = var[i];
	buffer[i] = '=';
	last = i + 1;
	for(i = 0; val[i]; i++)
		buffer[last + i] = val[i];
	buffer[last + i] = '\0';

	/* Set */
	pam_putenv(pamhandle, buffer);
	free(buffer);
}

#define PATH_VAR "/usr/local/sbin:/usr/local/bin:/usr/bin"
static void set_env(struct passwd *pw) {
	set_var("HOME", pw->pw_dir);
	set_var("PWD", pw->pw_dir);
	set_var("SHELL", pw->pw_shell);
	set_var("USER", pw->pw_name);
	set_var("LOGNAME", pw->pw_name);
	set_var("PATH", PATH_VAR);
}

int authenticate(const char *uname, const char *pword) {
	/* Format the way PAM likes */
	const char *credentials[2] = { uname, pword };
	int result;
	struct pam_conv conv = {
		pam_conv_cb,
		credentials
	};
	
	/* Start PAM */
	result = pam_start(servicename, uname, &conv, &pamhandle);
	if(result != PAM_SUCCESS)
		return panic(PANIC_PAM_START);
	/* Authenticate */
	result = pam_authenticate(pamhandle, 0x0);
	if(result != PAM_SUCCESS) {
		panic_data_push((char *)uname, strlen(uname) + 1);
		panic_data_push((char *)pword, strlen(pword) + 1);
		return panic(PANIC_PAM_AUTHENTICATION);
	}
	/* Verify */
	result = pam_acct_mgmt(pamhandle, 0);
	if(result != PAM_SUCCESS)
		return panic(PANIC_PAM_VERIFY);
	/* Establish credentials */
	result = pam_setcred(pamhandle, PAM_ESTABLISH_CRED);
	if(result != PAM_SUCCESS)
		return panic(PANIC_PAM_CREDENTIALS);

	username = uname;
	return PANIC_NONE;
}

int login() {
	int result;

	result = pam_open_session(pamhandle, 0);
	if(result != PAM_SUCCESS) {
		pam_setcred(pamhandle, PAM_DELETE_CRED);
		return panic(PANIC_PAM_SESSION);
	}

	struct passwd *pw = getpwnam(username);
	set_env(pw);
	return PANIC_NONE;
}

int logout() {
	int result;

	result = pam_close_session(pamhandle, 0);
	if(result != PAM_SUCCESS) {
		pam_setcred(pamhandle, PAM_DELETE_CRED);

		return panic(PANIC_PAM_SESSION_CLOSE);
	}

	result = pam_setcred(pamhandle, PAM_DELETE_CRED);
	if(result != PAM_SUCCESS)
		return panic(PANIC_PAM_CREDENTIALS_DELETE);


	pam_end(pamhandle, result);
	pamhandle = NULL;
	return result;
}





