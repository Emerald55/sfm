#include <algorithm>
#include <math.h>
#include "left_pane.h"
#include "file_io.h"
#include "screen_info.h"

left_pane::left_pane(unsigned int y, unsigned int x,
	       	bool show_symbolic_links, bool show_hidden_files) {
	pane = newwin(y, x, 0, 0);
	this->show_symbolic_links = show_symbolic_links;
	this->show_hidden_files = show_hidden_files;
}

void left_pane::update(screen_info &scr, bool draw_right_pane,
	       	const std::string &search_str, const std::string &current_path) {
	box(pane, 0, 0);
	files = file_io::get_dir_files(current_path, show_hidden_files, search_str);
	std::sort(files.begin(), files.end());
	size = files.size();
	const unsigned int current_line_number = (scr.page - scr.term_height) + scr.curs_y + 1;
	if (size < current_line_number) { //if cursor was over a file and it was deleted
		scr.page = static_cast<unsigned int>(std::ceil(static_cast<double>(size)
					/ static_cast<double>(scr.term_height))) * scr.term_height;
	}
	if (size != 0) {
		const unsigned int page_floor = scr.page - scr.term_height;
		files.erase(files.begin(), files.begin() + page_floor); //trim before page
		if (size > scr.page) {
			files.erase(files.begin() + scr.page - page_floor, files.end()); //trim after
		}
	}
	if (size < current_line_number) { //set cursor to last available file after trim
		scr.curs_y = files.size() - 1;
	}
	if (size != 0) {
		selected_filepath = files[scr.curs_y];
	}
	draw_window_files(scr, draw_right_pane, true);
	draw_window_title(current_path, scr, draw_right_pane);
	if (!draw_right_pane) {
		draw_window_info(scr, size, selected_filepath, draw_right_pane);
	}
	wrefresh(pane);
}
