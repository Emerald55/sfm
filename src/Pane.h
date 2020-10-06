#ifndef PANE_H
#define PANE_H
#include <ncurses.h>
#include <vector>
#include <string>
#include "Screen.h"
#include "FlagParse.h"

class Pane {
	public:
		~Pane();
		void resize(unsigned int width, unsigned int height);
		void reposition(unsigned int x, unsigned int y);
	protected:
		WINDOW* pane;
		unsigned int width;
		unsigned int height;
		std::vector<std::string> files;
		void draw_window_files(const Screen &scr, const FlagParse &flags, bool draw_curs = false) const;
		void draw_window_info(const Screen &scr, unsigned int current_dir_size,
				const std::string &current_filepath, bool searching) const;
		void draw_window_title(std::string path) const;
};

#endif
