#ifndef KEYBINDS_H
#define KEYBINDS_H
#include "screen_info.h"

class keybinds {
	public:
		std::string search_str;
		bool quit(const screen_info &scr) const;
		void move_left(screen_info &scr);
		void move_right(screen_info &scr, const std::string &selected_filepath);
		void move_up(screen_info &scr) const;
		void move_down(screen_info &scr, size_t left_pane_size_currently, 
				size_t left_pane_size) const;
		void jump_to_bottom(screen_info &scr, size_t left_pane_size_currently) const;
		void up_page(screen_info &scr) const;
		void down_page(screen_info &scr, size_t left_pane_size) const;
		void jump_to_line(screen_info &scr, size_t left_pane_size) const;
		void edit_text(const std::string &selected_filepath) const;
		void pager(const std::string &selected_filepath) const;
		void spawn_shell() const;
		void xdg_open(const std::string &selected_filepath) const;
		void remove(const screen_info &scr, const std::string &selected_filepath) const;
		void rename(const screen_info &scr, const std::string &selected_filepath) const;
		void copy(const screen_info &scr, const std::string &selected_filepath);
		void cut(const screen_info &scr, const std::string &selected_filepath);
		void paste(screen_info &scr, const std::string &current_path);
		void search(screen_info &scr);
		void screen_change(const screen_info &scr, WINDOW* left_pane, bool &draw_right_pane) const;
		void help(const screen_info &scr) const;
	private:
		bool cut_path = false;
		std::string copy_path;
};

#endif
