#ifndef _BDM_WM_LIST_H_
#define _BDM_WM_LIST_H_

void  wml_enumerate();
void  wml_free();

uint64_t    wml_length();
const char *wml_index(uint64_t i);

#endif
