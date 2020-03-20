#ifndef KEYBINDS_H
#define KEYBINDS_H
#include "user_interface.h"

class keybinds {
	public:
		user_interface *ui;
		keybinds(user_interface *ui);
		std::string search_str;
		bool quit();
		void move_left();
		void move_right(const std::string &selected_filepath);
		void move_up();
		void move_down(unsigned int current_dir_size_currently, 
				size_t current_dir_size);
		void jump_to_bottom(unsigned int current_dir_size_currently);
		void up_page();
		void down_page(unsigned int current_dir_size_currently, 
				size_t current_dir_size);
		void jump_to_line(size_t current_dir_size);
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
	private:
		bool cut_path = false;
		std::string copy_path;
		inline int round_to(int num, int multiple) {
			if (multiple == 0) { return num; }
			int remainder = num % multiple;
			if (remainder == 0) { return num; }
			return num + multiple - remainder;
		}
};

#endif
