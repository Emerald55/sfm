#ifndef LEFTPANE_H
#define LEFTPANE_H
#include <string>
#include <ncurses.h>
#include "Pane.h"
#include "RightPane.h"
#include "Screen.h"
#include "Keybinds.h"
#include "FlagParse.h"

class LeftPane : public Pane {
	public:
		LeftPane(unsigned int width, unsigned int height);
		void update(Screen &scr, bool draw_right_pane, const FlagParse &flags);
		bool get_input(Screen &scr, RightPane &rp);
		inline std::string get_selected_filepath() const { return selected_filepath; }
		inline size_t get_size() const { return size; }
		inline bool get_searching() const { return searching; }
	private:
		Keybinds kb;
		size_t size;
		bool searching = false;
		std::string selected_filepath;
		std::string current_path;
};

#endif
