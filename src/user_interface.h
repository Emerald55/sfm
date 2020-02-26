#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

class user_interface {
	public:
		unsigned int scr_y, scr_x;
		int curs_y = 0;
		unsigned int term_height;
		unsigned int page;
		bool draw_selected_path = true;
		user_interface() {
			getmaxyx(stdscr, scr_y, scr_x);
			term_height = scr_y - 2;
			page = 1 * term_height;
		}
		void draw_window(const std::string &path, std::vector<std::string> files, WINDOW *win, 
				std::vector<std::string> file_contents, const int &argc, char* argv[],
			       	const bool &draw_curs);
		void draw_info(WINDOW *win, const unsigned int &page, const unsigned int &current_dir_size);
		std::string input(const char* text, const unsigned int &win_width, const unsigned int &color_type);
		void alert_box(const char* text, const unsigned int &win_width, const unsigned int &sleep_time,
			       	const unsigned int &alert_color);
	private:
		void draw_window_title(const std::string &path, WINDOW *win);
};

#endif
