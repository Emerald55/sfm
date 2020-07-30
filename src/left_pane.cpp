#include <algorithm>
#include <string>
#include <cmath>
#include "left_pane.h"
#include "file_io.h"
#include "screen_info.h"
#include "flag_parse.h"

left_pane::left_pane(unsigned int y, unsigned int x) {
	pane = newwin(y, x, 0, 0);
}

void left_pane::update(screen_info &scr, bool draw_right_pane,
	       	const std::string &search_str, const flag_parse &flags) {
	current_path = std::filesystem::current_path().string();
	files.clear();
	size = 0;
	selected_filepath = "?";
	box(pane, 0, 0);
	files = file_io::get_dir_files(current_path, flags, search_str);
	std::sort(files.begin(), files.end());
	size = files.size();
	if (size != 0) {
		const unsigned int current_line_number = (scr.get_page() - scr.get_term_height())
		       	+ scr.get_curs_y() + 1;
		if (size < current_line_number) { //if deleted last file on a page
			scr.set_page(static_cast<unsigned int>(std::ceil(static_cast<double>(size)
						/ static_cast<double>(scr.get_term_height()))) * scr.get_term_height());
		}
		const unsigned int page_floor = scr.get_page() - scr.get_term_height();
		files.erase(files.begin(), files.begin() + page_floor); //trim before page
		if (size > scr.get_page()) {
			files.erase(files.begin() + scr.get_page() - page_floor, files.end()); //trim after
		}
		if (size < current_line_number) { //if hovering over last file and its deleted
			scr.set_curs_y(files.size() - 1);
		}
		selected_filepath = files[scr.get_curs_y()];
	}
	draw_window_files(scr, draw_right_pane, flags, true);
	draw_window_title(current_path, scr, draw_right_pane);
	if (!draw_right_pane) {
		draw_window_info(scr, size, selected_filepath, draw_right_pane);
	}
	wrefresh(pane);
}
