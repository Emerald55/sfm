#ifndef KEYBINDS_H
#define KEYBINDS_H
#include "user_interface.h"

class keybinds {
	public:
		std::string search_str;
		keybinds(user_interface *ui);
		void get_input(const std::string &selected_filepath,
			       	unsigned int left_pane_size_currently,
			       	unsigned int left_pane_size, 
				const std::string &current_path, 
				bool &is_running);
	private:
		user_interface *ui;
		bool cut_path = false;
		std::string copy_path;
		bool quit();
		void move_left();
		void move_right(const std::string &selected_filepath);
		void move_up();
		void move_down(unsigned int left_pane_size_currently, 
				size_t left_pane_size);
		void jump_to_bottom(unsigned int left_pane_size_currently);
		void up_page();
		void down_page(unsigned int left_pane_size_currently, 
				size_t left_pane_size);
		void jump_to_line(size_t left_pane_size);
		void edit_text(const std::string &selected_filepath);
		void pager(const std::string &selected_filepath);
		void spawn_shell();
		void xdg_open(const std::string &selected_filepath);
		void remove(const std::string &selected_filepath);
		void rename(const std::string &selected_filepath);
		void copy(const std::string &selected_filepath);
		void cut(const std::string &selected_filepath);
		void paste(const std::string &current_path);
		void search();
		void screen_change();
		void help();
		inline int round_to(int num, int multiple) {
			if (multiple == 0) { return num; }
			int remainder = num % multiple;
			if (remainder == 0) { return num; }
			return num + multiple - remainder;
		}
};

#endif
