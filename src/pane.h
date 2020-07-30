#ifndef PANE_H
#define PANE_H
#include <ncurses.h>
#include <vector>
#include <string>
#include "screen_info.h"
#include "flag_parse.h"

class pane {
	public:
		WINDOW* pane;
		~pane();
		inline std::vector<std::string> get_files() const { return files; }
	protected:
		std::vector<std::string> files;
		void draw_window_files(const screen_info &scr, bool draw_right_pane,
			       	const flag_parse &flags, bool draw_curs = false) const;
		void draw_window_info(const screen_info &scr, unsigned int current_dir_size,
				const std::string &current_filepath, bool draw_right_pane) const;
		void draw_window_title(std::string path, const screen_info &scr,
				bool draw_right_pane) const;
};

#endif
