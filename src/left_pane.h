#ifndef LEFT_PANE_H
#define LEFT_PANE_H
#include <string>
#include <ncurses.h>
#include "pane.h"
#include "screen_info.h"
#include "flag_parse.h"

struct left_pane : public pane {
	size_t size;
	std::string selected_filepath;
	left_pane(unsigned int y, unsigned int x);
	void update(screen_info &scr, bool draw_right_pane, const std::string &search_str,
		       	const std::string &current_path, const flag_parse &flags);
};

#endif
