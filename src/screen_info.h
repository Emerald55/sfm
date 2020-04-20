#ifndef SCREEN_INFO_H
#define SCREEN_INFO_H
#include <ncurses.h>

struct screen_info {
	unsigned int y;
	unsigned int x;
	unsigned int term_height;
	unsigned int curs_y = 0;
	unsigned int page;
	const unsigned int update_speed = 250;
	screen_info();
	void check_resize(WINDOW* left_pane, WINDOW* right_pane, bool draw_right_pane);
};

#endif
