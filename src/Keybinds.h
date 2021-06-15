#ifndef KEYBINDS_H
#define KEYBINDS_H
#include <string>
#include "Screen.h"

class Keybinds {
	public:
		inline std::string get_search_str() const { return search_str; }
		bool quit(const Screen &scr) const;
		bool move_left(Screen &scr);
		bool move_right(Screen &scr, const std::string &selected_filepath);
		void move_up(Screen &scr) const;
		void move_down(Screen &scr, size_t left_pane_size_currently, 
				size_t left_pane_size) const;
		void jump_to_bottom(Screen &scr, size_t left_pane_size_currently) const;
		void up_page(Screen &scr) const;
		void down_page(Screen &scr, size_t left_pane_size) const;
		void jump_to_line(Screen &scr, size_t left_pane_size) const;
		void edit_text(const std::string &selected_filepath) const;
		void pager(const std::string &selected_filepath) const;
		void spawn_shell() const;
		void xdg_open(const std::string &selected_filepath) const;
		void remove(const Screen &scr, const std::string &selected_filepath) const;
		void rename(const Screen &scr, const std::string &selected_filepath) const;
		void copy(const Screen &scr, const std::string &selected_filepath);
		void cut(const Screen &scr, const std::string &selected_filepath);
		void paste(Screen &scr, const std::string &current_path);
		bool search(Screen &scr);
		bool screen_change(const Screen &scr, LeftPane *lp, bool draw_right_pane) const;
		void help(const Screen &scr) const;
	private:
		bool cut_path = false;
		std::string copy_path;
		std::string search_str;
};

#endif
