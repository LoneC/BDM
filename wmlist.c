#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "panic.h"
#include <dirent.h>
#include <sys/types.h>

const char * const WMLIST_PATH = "wm";
#define MORE_LIST                  8
#define MORE_BUFFER               32

typedef struct _wm_s {
	char *name;
	char *desc;
	char *exec;
} wm_t;

static wm_t     *wmlist;
static uint64_t  wmlen;
static uint64_t  wmmax;

static void trim_string(char *dst) {
	uint64_t start = 0, end = strlen(dst), i;
	while(dst[start] == ' ') start++;
	while(dst[end - 1] == ' ') end--;
	for(i = 0; i < end - start; i++)
		dst[i] = dst[start + i];
	dst[i] = '\0';
}

static int name_applicable(const char *s) {
	uint64_t len = strlen(s);
	if(len <= 3)
		return 0;
	if(
		s[len - 1] != 'm' && s[len - 2] != 'w' &&
		s[len - 1] != 'M' && s[len - 2] != 'W' &&
		s[len - 3] != '.'
	) return 0;
	return 1;
}

static int parse_and_add(FILE *file) {
	/* If ever a syntax error, just skip */	
	/* No need to shut down whole program, CLI will still be an option always */
	char c;
	char *buffer = malloc(MORE_BUFFER);
	uint64_t buffermax = MORE_BUFFER;
	uint64_t bufferlen = 0;
	uint64_t i;
#define NAME 0
#define DESC 1
#define EXEC  2
	int name = 0, desc = 0, exec = 0;
	int current = 0;
	wm_t wm = { NULL, NULL, NULL };


	while(!feof(file)) {
		/* Read any space away */
		do {
			i = ftell(file);
			c = fgetc(file);
		} while(c == ' ' || c == '\t' || c == '\n');
		if(feof(file))
			break;
		fseek(file, i, SEEK_SET);
		
		/* Read left side, read until '=' */
		bufferlen = 0;
		do {
			c = fgetc(file);
			buffer[bufferlen++] = c;
			if(bufferlen >= buffermax) {
				buffermax += MORE_BUFFER;
				buffer = realloc(buffer, buffermax);
			}
		} while(c != '=' && !feof(file));
		if(feof(file))
			break;
		
		/* Last index is an '=', there may be a space inbetween depending on how user wrote it. */
		/* Cut out last char, and trim. The -1 is for the '=' */
		buffer[bufferlen - 1] = '\0';
		trim_string(buffer);
		
		if(strncmp("name", buffer, 5) == 0) {
			name = 1;
			current = NAME;
		}
		else if(strncmp("desc", buffer, 5) == 0) {
			desc = 1;
			current = DESC;
		}
		if(strncmp("exec", buffer, 5) == 0) {
			exec = 1;
			current = EXEC;
		}

		/* Read the right side, ever single char inbetween the two quotes */
		/* First, there may be space in between the equals that was already read and the first quote */
		
		do {
			c = fgetc(file);
		} while(c != '"' && !feof(file));
		if(feof(file))
			break;
		
		/* Now read until next quote */
		bufferlen = 0;
		do {
			c = fgetc(file);
			buffer[bufferlen++] = c;
			if(bufferlen >= buffermax) {
				buffermax += MORE_BUFFER;
				buffer = realloc(buffer, buffermax);
			}
		} while(c != '"' && !feof(file));
		buffer[bufferlen - 1] = '\0'; /* -1 for cutting off quote */
		trim_string(buffer);
		
		/* Now set the data */
		switch(current) {
			case NAME: wm.name = strdup(buffer); break;
			case DESC: wm.desc = strdup(buffer); break;
			case EXEC: wm.exec = strdup(buffer); break;
			default: panic(PANIC_MAX);
		}
	}
	
	/* If all data was set, it is safe to add to the list */
	if(!name && !desc && !exec) {
		if(wm.name != NULL) free(wm.name);
		if(wm.desc != NULL) free(wm.desc);
		if(wm.exec != NULL) free(wm.exec);
		free(buffer);
		return 0;
	}

	wmlist[wmlen++] = wm;
	if(wmlen >= wmmax) {
		wmmax += MORE_LIST;
		wmlist = realloc(wmlist, wmmax);
	}

	free(buffer);
	return 1;
}

static void attempt_add(struct dirent *d) {
	if(!name_applicable(d->d_name))
		return;
	char *path = malloc(strlen(WMLIST_PATH) + strlen(d->d_name) + 1);
	uint64_t i, last;
	for(i = 0; WMLIST_PATH[i]; i++)
		path[i] = WMLIST_PATH[i];
	path[i] = '/';
	last = i + 1;
	for(i = 0; d->d_name[i]; i++)
		path[last + i] = d->d_name[i];
	path[last + i] = '\0';

	FILE *file = fopen(path, "r");
	if(file == NULL) {
		panic(PANIC_WMLIST_CONCATENATE);
		return;
	}
	parse_and_add(file);
	fclose(file);
	free(path);
}

void wml_enumerate() {
	wmlist = malloc(sizeof(wm_t) * MORE_LIST);
	wmmax = MORE_LIST;
	wmlen = 0;
	DIR *dir = opendir(WMLIST_PATH);
	if(dir == NULL) {
		panic(PANIC_WMLIST_INVALID_SEARCH_DIR);
		free(wmlist);
		return;
	}
	struct dirent *d;

	while((d = readdir(dir)) != NULL)
		attempt_add(d);

	closedir(dir);

	/* After adding all, add the CLI as final option */
	wm_t wm = {
		.name = strdup("CLI"),
		.desc = strdup("No window manager; command line interface"),
		.exec = NULL
	};
	wmlist[wmlen++] = wm;
}

void wml_free() {
	uint64_t i;
	for(i = 0; i < wmlen; i++) {
		if(wmlist[i].name != NULL)
			free(wmlist[i].name);
		if(wmlist[i].desc != NULL)
			free(wmlist[i].desc);
		if(wmlist[i].exec != NULL)
			free(wmlist[i].exec);
	}
	free(wmlist);
	wmlen = 0;
	wmmax = 0;
	wmlist = NULL;

}

uint64_t wml_length() {
	return wmlen;
}

const char *wml_index(uint64_t i) {
	if(i >= wmlen)
		return "<nil>";
	return wmlist[i].name;
}



