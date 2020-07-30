#ifndef SCREEN_INFO_H
#define SCREEN_INFO_H
#include <ncurses.h>

class screen_info {
	public:
		const unsigned int update_speed = 250;
		screen_info();
		void check_resize(WINDOW* left_pane, WINDOW* right_pane, bool draw_right_pane);
		void reset_to_first_page();
		inline unsigned int get_y() const { return y; }
		inline unsigned int get_x() const { return x; }
		inline unsigned int get_term_height() const { return term_height; };
		inline unsigned int get_curs_y() const { return curs_y; };
		inline void set_curs_y(unsigned int curs_y) { this->curs_y = curs_y; }
		inline unsigned int get_page() const { return page; };
		inline void set_page(unsigned int page) { this->page = page; }
	private:
		unsigned int y;
		unsigned int x;
		unsigned int term_height;
		unsigned int curs_y = 0;
		unsigned int page;
};

#endif
