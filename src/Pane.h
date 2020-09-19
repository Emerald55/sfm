#ifndef PANE_H
#define PANE_H
#include <ncurses.h>
#include <vector>
#include <string>
#include "Screen.h"
#include "FlagParse.h"

class Pane {
	public:
		~Pane();
		void resize(unsigned int width, unsigned int height);
		void reposition(unsigned int x, unsigned int y);
		inline std::vector<std::string> get_files() const { return files; }
		inline unsigned int get_width() const { return width; }
		inline unsigned int get_height() const { return height; }
	protected:
		WINDOW* pane;
		unsigned int width;
		unsigned int height;
		std::vector<std::string> files;
		void draw_window_files(const Screen &scr, const FlagParse &flags, bool draw_curs = false) const;
		void draw_window_info(const Screen &scr, unsigned int current_dir_size,
				const std::string &current_filepath, size_t search_length) const;
		void draw_window_title(std::string path) const;
};

#endif
