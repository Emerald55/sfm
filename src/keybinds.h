#ifndef KEYBINDS_H
#define KEYBINDS_H
#include "user_interface.h"

class keybinds {
	public:
		std::string search_str;
		void move_left(user_interface &ui);
		void move_right(user_interface &ui, const std::string &selected_filepath);
		void move_up(user_interface &ui, const unsigned int current_dir_size_currently);
		void move_down(user_interface &ui, const unsigned int current_dir_size_currently, 
				const size_t &current_dir_size);
		void jump_to_top(user_interface &ui);
		void jump_to_bottom(user_interface &ui,
			       	const unsigned int current_dir_size_currently);
		void up_page(user_interface &ui);
		void down_page(user_interface &ui, const unsigned int current_dir_size_currently, 
				const size_t &current_dir_size);
		void jump_to_line(user_interface &ui, const size_t &current_dir_size);
		void edit_text(const std::string &selected_filepath);
		void spawn_shell();
		void xdg_open(const std::string &selected_filepath, const size_t &current_dir_size);
		void remove(user_interface &ui, const std::string &selected_filepath);
		void rename(user_interface &ui, const std::string &selected_filepath, 
				const unsigned int current_dir_size_currently);
		void copy(user_interface &ui, const std::string &selected_filepath, 
				const unsigned int current_dir_size_currently);
		void cut(user_interface &ui, const std::string &selected_filepath);
		void paste(user_interface &ui, const std::string &current_path);
		void search(user_interface &ui);
		void screen_change(user_interface &ui, WINDOW *current_dir_win);
		void help(user_interface &ui, const unsigned int &update_speed);
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
