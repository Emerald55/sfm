#ifndef RIGHT_PANE_H
#define RIGHT_PANE_H
#include <vector>
#include <string>
#include <ncurses.h>
#include "pane.h"
#include "screen_info.h"

class right_pane : public pane {
	public:
		std::vector<std::string> file_content;
		bool draw = false;
		right_pane(unsigned int x, bool show_symbolic_links, bool show_hidden_files);
		void update(const screen_info &scr, const std::string &selected_filepath, size_t left_pane_size);
	private:
		void draw_window_file_contents(const screen_info &scr, bool contents_printable) const;
};

#endif
