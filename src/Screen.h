#ifndef SCREEN_H
#define SCREEN_H
#include <optional>
#include <ncurses.h>

class RightPane;
class LeftPane;

class Screen {
	public:
		const unsigned int update_speed = 250;
		Screen();
		void check_resize(LeftPane &lp, RightPane &rp);
		void reset_to_first_page();
		void set(std::optional<unsigned int> curs_y = std::nullopt, 
				std::optional<unsigned int> page = std::nullopt);
		inline unsigned int get_y() const { return y; }
		inline unsigned int get_x() const { return x; }
		inline unsigned int get_term_height() const { return term_height; };
		inline unsigned int get_curs_y() const { return curs_y; };
		inline unsigned int get_page() const { return page; };
	private:
		unsigned int y;
		unsigned int x;
		unsigned int term_height;
		unsigned int curs_y = 0;
		unsigned int page;
};

#endif
