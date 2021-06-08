#include <ncurses.h>
#include "Screen.h"
#include "LeftPane.h"
#include "RightPane.h"

Screen::Screen() {
	start_color();
	init_pair(1, COLOR_WHITE, COLOR_RED); //cursor
	init_pair(2, COLOR_YELLOW, COLOR_BLACK); //directories
	init_pair(3, COLOR_CYAN, COLOR_BLACK); //line numbers
	init_pair(4, COLOR_GREEN, COLOR_BLACK); //green input box
	init_pair(5, COLOR_RED, COLOR_BLACK); //red input box and unprintable warning
	init_pair(6, COLOR_YELLOW, COLOR_RED); //cursor on directory
	init_pair(7, COLOR_WHITE, COLOR_MAGENTA); //unknown filetype
	cbreak();
	noecho();
	curs_set(0);
	keypad(stdscr, true);
	getmaxyx(stdscr, y, x);
	term_height = y - 2;
	page = term_height;
}

void Screen::check_resize(LeftPane &lp, RightPane &rp) {
	unsigned int check_scr_y, check_scr_x;
	getmaxyx(stdscr, check_scr_y, check_scr_x);
	if (check_scr_y > 2 && check_scr_x > 2 &&
		       	(check_scr_y != y || check_scr_x != x)) { //too small of a screen to split into two
		if (check_scr_y != y) {
			y = check_scr_y;
			const unsigned int page_number = page / term_height;
			term_height = y - 2;
			page = term_height * page_number;
			if (curs_y >= term_height) { //if cursor is farther out than height from resizing
				curs_y = term_height - 1;
			}
		}
		if (check_scr_x != x) {
			x = check_scr_x;
		}
		if (rp.get_draw()) {
			lp.resize(x / 2, y);
		}
		else {
			lp.resize(x, y);
		}
		rp.resize(x / 2 + (x % 2), y);
		rp.reposition(x / 2, 0);
	}
}

void Screen::reset_to_first_page() {
	curs_y = 0;
	page = term_height;
}

void Screen::set(std::optional<unsigned int> curs_y, std::optional<unsigned int> page) {
	if (page) {
		this->page = page.value();
	}
	if (curs_y) {
		this->curs_y = curs_y.value();
	}
}
