#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "userstring.h"
#include "frontend.h"
#include "panic.h"


int main(int argc, const char **argv) {
	frontend_init();

	while(panicked() == PANIC_NONE && !frontend_finished()) {
		frontend_frame();
		frontend_await_input();
	}
	frontend_exit();

	unsigned int i;
	if(panicked()) {
		puts(panic_string());
		puts("---");
		for(i = 0; i < panic_data_len(); i++)
			printf(" > '%s'\n", (char *)panic_data_get(i));
		panic_data_flush();
		return panicked();
	}

	return 0;
}




