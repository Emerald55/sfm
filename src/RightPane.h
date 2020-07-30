#ifndef RIGHTPANE_H
#define RIGHTPANE_H
#include <vector>
#include <string>
#include <ncurses.h>
#include "Pane.h"
#include "Screen.h"
#include "FlagParse.h"

class RightPane : public Pane {
	public:
		RightPane(unsigned int x);
		void update(const Screen &scr, const std::string &selected_filepath, size_t left_pane_size, 
				const FlagParse &flags);
		inline bool get_draw() const { return draw; }
		inline void set_draw(bool draw) { this->draw = draw; }
	private:
		void draw_window_file_contents(const Screen &scr, bool contents_printable) const;
		std::vector<std::string> file_content;
		bool draw = false;
};

#endif
