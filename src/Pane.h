#ifndef PANE_H
#define PANE_H
#include <ncurses.h>
#include <vector>
#include <string>
#include "Screen.h"
#include "FlagParse.h"

class Pane {
	public:
		WINDOW* pane;
		~Pane();
		inline std::vector<std::string> get_files() const { return files; }
	protected:
		std::vector<std::string> files;
		void draw_window_files(const Screen &scr, bool draw_right_pane,
			       	const FlagParse &flags, bool draw_curs = false) const;
		void draw_window_info(const Screen &scr, unsigned int current_dir_size,
				const std::string &current_filepath, bool draw_right_pane) const;
		void draw_window_title(std::string path, const Screen &scr,
				bool draw_right_pane) const;
};

#endif
