#ifndef KEYBINDS_H
#define KEYBINDS_H
#include "user_interface.h"

class keybinds {
	public:
		user_interface *ui;
		keybinds(user_interface *ui) {
			this->ui = ui;
		}
		std::string search_str;
		void move_left();
		void move_right(const std::string &selected_filepath);
		void move_up(const unsigned int current_dir_size_currently);
		void move_down(const unsigned int current_dir_size_currently, 
				const size_t &current_dir_size);
		void jump_to_top();
		void jump_to_bottom(const unsigned int current_dir_size_currently);
		void up_page();
		void down_page(const unsigned int current_dir_size_currently, 
				const size_t &current_dir_size);
		void jump_to_line(const size_t &current_dir_size);
		void edit_text(const std::string &selected_filepath,
			       	const size_t &current_dir_size);
		void spawn_shell();
		void xdg_open(const std::string &selected_filepath,
			       	const size_t &current_dir_size);
		void remove(const std::string &selected_filepath);
		void rename(const std::string &selected_filepath, 
				const unsigned int current_dir_size_currently);
		void copy(const std::string &selected_filepath, 
				const unsigned int current_dir_size_currently);
		void cut(const std::string &selected_filepath);
		void paste(const std::string &current_path);
		void search();
		void screen_change();
		void help();
	private:
		bool cut_path = false;
		std::string copy_path;
		inline int round_to(const int num, const int multiple) {
			if (multiple == 0) { return num; }
			int remainder = num % multiple;
			if (remainder == 0) { return num; }
			return num + multiple - remainder;
		}
};

#endif
