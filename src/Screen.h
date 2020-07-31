#ifndef SCREEN_H
#define SCREEN_H
#include <ncurses.h>

class RightPane;
class LeftPane;

class Screen {
	public:
		const unsigned int update_speed = 250;
		Screen();
		void check_resize(LeftPane &lp, RightPane &rp);
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
