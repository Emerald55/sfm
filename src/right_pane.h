#ifndef RIGHT_PANE_H
#define RIGHT_PANE_H
#include <vector>
#include <string>
#include <ncurses.h>
#include "pane.h"
#include "screen_info.h"
#include "flag_parse.h"

class right_pane : public pane {
	public:
		right_pane(unsigned int x);
		void update(const screen_info &scr, const std::string &selected_filepath, size_t left_pane_size, 
				const flag_parse &flags);
		inline bool get_draw() const { return draw; }
		inline void set_draw(bool draw) { this->draw = draw; }
	private:
		void draw_window_file_contents(const screen_info &scr, bool contents_printable) const;
		std::vector<std::string> file_content;
		bool draw = false;
};

#endif
