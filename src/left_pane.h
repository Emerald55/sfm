#ifndef LEFT_PANE_H
#define LEFT_PANE_H
#include <string>
#include <ncurses.h>
#include "pane.h"
#include "screen_info.h"
#include "flag_parse.h"

class left_pane : public pane {
	public:
		left_pane(unsigned int y, unsigned int x);
		void update(screen_info &scr, bool draw_right_pane, const std::string &search_str,
				const flag_parse &flags);
		inline std::string get_selected_filepath() const { return selected_filepath; }
		inline std::string get_current_path() const { return current_path; }
		inline size_t get_size() const { return size; }
	private:
		size_t size;
		std::string selected_filepath;
		std::string current_path;
};

#endif
