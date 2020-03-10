#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

class user_interface {
	public:
		unsigned int scr_y, scr_x;
		int curs_y = 0;
		unsigned int term_height;
		unsigned int page;
		bool draw_selected_path = false;
		WINDOW *current_dir_win;
		WINDOW *selected_dir_win;
		const unsigned int update_speed = 100;
		user_interface() {
			getmaxyx(stdscr, scr_y, scr_x);
			setlocale(LC_ALL, "");
			start_color();
			init_pair(1, COLOR_WHITE, COLOR_RED); //cursor
			init_pair(2, COLOR_YELLOW, COLOR_BLACK); //directories
			init_pair(3, COLOR_CYAN, COLOR_BLACK); //line numbers
			init_pair(4, COLOR_GREEN, COLOR_BLACK); //green input box
			init_pair(5, COLOR_RED, COLOR_BLACK); //red input box
			init_pair(6, COLOR_BLACK, COLOR_GREEN); //help menu
			cbreak();
			noecho();
			curs_set(0);
			keypad(stdscr, true);
			timeout(update_speed);
			current_dir_win = newwin(scr_y, scr_x, 0, 0);
			selected_dir_win = newwin(0, 0, 0, scr_x / 2);
			term_height = scr_y - 2;
			page = 1 * term_height;
		}
		void draw_window_files(const std::string &path, const std::vector<std::string> &files, WINDOW *win, 
				const int &argc, char* argv[], const bool &draw_curs = false);
		void draw_window_file_contents(const std::string &path, WINDOW *win,
			       	std::vector<std::string> file_contents);
		void draw_info(WINDOW *win, const unsigned int &page, const unsigned int &current_dir_size);
		std::string input(const char* text, const unsigned int &win_width, const unsigned int &color_type);
		void alert_box(const char* text, const unsigned int &win_width, const unsigned int &sleep_time,
			       	const unsigned int &alert_color);
		void check_resize();
	private:
		void draw_window_title(std::string path, WINDOW *win);
};

#endif
