#include "frontend.h"

#include "panic.h"
#include "userstring.h"
#include "wmlist.h"
#include "auth.h"
#include <string.h>
#include <stdbool.h>
#include <curses.h>

enum selection {
	SELECTION_WINDOWMANAGER = 0,
	SELECTION_USERNAME,
	SELECTION_PASSWORD,
	SELECTION_POWEROFF,
	SELECTION_REBOOT,
	SELECTION_MAX
} sel = SELECTION_USERNAME;

static usrstr uname;
static usrstr pword;
static int xuname, yuname;
static int xpword, ypword;
static int xwinmgr, ywinmgr;
static int xpoweroff, ypoweroff;
static int xreboot, yreboot;
static uint64_t wmlen;
static int wmindex = 0;
static bool finished = false;

#include "config.h"

void input_winmgr(int c, void *ud) {
	switch(c) {
		case KEY_LEFT:
			if(wmindex == 0) {
				wmindex = wmlen - 1;
				break;
			}
			wmindex--;
			break;
		case KEY_RIGHT:
			wmindex++;
			if(wmindex >= wmlen)
				wmindex = 0;
			break;
		default: break;
	}
}

void input_text(int c, usrstr *us) {
	switch(c) {
		case KEY_BACKSPACE:
			usrstr_pop(us);
			break;
		case '\n':
			if(sel == SELECTION_PASSWORD) {
				panic_data_push(uname.buffer, uname.len + 1);
				panic_data_push(pword.buffer, pword.len + 1);
				if(authenticate(uname.buffer, pword.buffer) != PANIC_NONE)
					return;
				else
					finished = true;
			}
			else sel++;
			break;
		case KEY_LEFT: case KEY_RIGHT:
			/* Keys to ignore */
			break;
		default:
			usrstr_push(us, (char)(c));
			break;
	}
}

static void input_poweroff(int c, void *ud) {

}

static void input_reboot(int c, void *ud) {

}

void frontend_init() {
	initscr();
	noecho();
	cbreak();
	keypad(stdscr, TRUE);
	usrstr_init(&uname);
	usrstr_init(&pword);
	wml_enumerate();
	wmlen = wml_length();
}

void frontend_exit() {
	usrstr_free(&uname);
	usrstr_free(&pword);
	wml_free();
	endwin();	
}

static void draw_box(int x, int y, int w, int h) {
	/* Lines of box */
	mvhline(y    , x    , 0, w);
	mvhline(y + h, x    , 0, w);
	mvvline(y    , x    , 0, h);
	mvvline(y    , x + w, 0, h);
	/* Fix corners */
	mvaddch(y    , x    , ACS_ULCORNER);
	mvaddch(y    , x + w, ACS_URCORNER);
	mvaddch(y + h, x    , ACS_LLCORNER);
	mvaddch(y + h, x + w, ACS_LRCORNER);
}

static void draw_menu() {
	draw_box(xmenu, ymenu, wmenu, hmenu);
	/* Window manager selection */
	const char *wmstr = wml_index(wmindex);
	xwinmgr = (xmenu + wmenu) / 2 - (strlen(wmstr) / 2); /* Left side starting point to write text so it comes out centered */
	ywinmgr = ymenu + 3;
	mvaddch(ywinmgr, xmenu         + 4, '<');
	mvaddch(ywinmgr, xmenu + wmenu - 4, '>');
	mvaddstr(ywinmgr, xwinmgr, wmstr);
	/* Username input */
	mvaddstr(ymenu + 5, xmenu + 5, "Username: ");
	addstr(uname.buffer);
	getyx(stdscr, yuname, xuname);
	/* Password */
	mvaddstr(ymenu + 7, xmenu + 5, "Password: ");
	getyx(stdscr, ypword, xpword);
	uint64_t i;
	for(i = 0; i < pword.len; i++)
		mvaddch(ymenu + 7, xpword + i, '*');
	getyx(stdscr, ypword, xpword);
}

static void draw_more() {
	draw_box(xmore, ymore, wmore, hmore);
	const char *poweroff = "<Power off [F1]>",
	      	   *reboot   = "<Reboot [F2]>";
	mvaddstr(ymore + hmore - 2, xmore + 2, poweroff);
	getyx(stdscr, ypoweroff, xpoweroff);
	xpoweroff -= 4;
	mvaddstr(ymore + hmore - 2, xmore + wmore - strlen(reboot) - 2, reboot);
	getyx(stdscr, yreboot, xreboot);
	xreboot -= 4;
}

static void update_cursor() {
	switch(sel) {
		case SELECTION_WINDOWMANAGER:
			move(ywinmgr, xwinmgr);
			break;
		case SELECTION_USERNAME:
			move(yuname, xuname);
			break;
		case SELECTION_PASSWORD:
			move(ypword, xpword);
			break;
		case SELECTION_POWEROFF:
			move(ypoweroff, xpoweroff);
			break;
		case SELECTION_REBOOT:
			move(yreboot, xreboot);
			break;
		default: break;
	}
}

void frontend_frame() {
	/* Clear */
	clear();

	draw_menu();	
	draw_more();
	update_cursor();

	/* Draw */
	refresh();
}

void frontend_await_input() {
	int c = getch();
	switch(c) {
		case KEY_UP:
			if(sel == 0) {
				sel = SELECTION_MAX - 1;
				break;
			}
			sel--;
			break;
		case KEY_DOWN: case '\t':
			sel++;
			if(sel >= SELECTION_MAX)
				sel = 0;
			break;
		default:
			switch(sel) {
				case SELECTION_WINDOWMANAGER: input_winmgr(c, NULL);   break;
				case SELECTION_USERNAME:      input_text(c, &uname);   break;
				case SELECTION_PASSWORD:      input_text(c, &pword);   break;
				case SELECTION_POWEROFF:      input_poweroff(c, NULL); break;
				case SELECTION_REBOOT:        input_reboot(c, NULL);   break;
				default: break;
			}
			break;
	}
}

bool frontend_finished() {
	return finished;
}






