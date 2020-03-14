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
		user_interface();
		void draw_window_files(const std::string &path, const std::vector<std::string> &files, WINDOW *win, 
				int argc, char* argv[], bool draw_curs = false);
		void draw_window_file_contents(const std::string &path, WINDOW *win,
			       	std::vector<std::string> file_contents);
		void draw_info(WINDOW *win, unsigned int page, unsigned int current_dir_size);
		std::string input(const char* text, unsigned int win_width, unsigned int color_type);
		void alert_box(const char* text, unsigned int win_width, unsigned int sleep_time,
			       	unsigned int alert_color);
		void check_resize();
	private:
		void draw_window_title(std::string path, WINDOW *win);
};

#endif
