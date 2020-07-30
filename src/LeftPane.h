#ifndef LEFTPANE_H
#define LEFTPANE_H
#include <string>
#include <ncurses.h>
#include "Pane.h"
#include "Screen.h"
#include "FlagParse.h"

class LeftPane : public Pane {
	public:
		LeftPane(unsigned int width, unsigned int height);
		void update(Screen &scr, bool draw_right_pane, const std::string &search_str,
				const FlagParse &flags);
		inline std::string get_selected_filepath() const { return selected_filepath; }
		inline std::string get_current_path() const { return current_path; }
		inline size_t get_size() const { return size; }
	private:
		size_t size;
		std::string selected_filepath;
		std::string current_path;
};

#endif
