#ifndef _BDM_FRONTEND_H_
#define _BDM_FRONTEND_H_

#include <stdbool.h>

void frontend_init();
void frontend_exit();
void frontend_frame();
void frontend_await_input();
bool frontend_finished();

#endif
