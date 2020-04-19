#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H
#include <ncurses.h>

class user_interface {
	public:
		unsigned int scr_y, scr_x;
		unsigned int curs_y = 0;
		unsigned int term_height;
		unsigned int page;
		WINDOW *left_pane;
		WINDOW *right_pane;
		bool draw_right_pane = false;
		const unsigned int update_speed = 250;
		user_interface();
		void draw_window_files(const std::vector<std::string> &files, WINDOW *win, 
				bool show_symbolic_links, bool draw_curs = false);
		void draw_window_file_contents(WINDOW *win, std::vector<std::string> file_contents,
			       	bool contents_printable);
		void draw_info(WINDOW *win, unsigned int current_dir_size, const std::string &current_filepath);
		std::string input(const std::string &text, unsigned int color_type, unsigned int win_width = 15);
		void alert_box(const char* text, unsigned int win_width, unsigned int sleep_time,
			       	unsigned int alert_color);
		void check_resize();
		void draw_window_title(std::string path, WINDOW *win);
};

#endif
