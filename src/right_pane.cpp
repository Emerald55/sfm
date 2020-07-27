#include <algorithm>
#include <filesystem>
#include <string>
#include "right_pane.h"
#include "file_io.h"
#include "screen_info.h"
#include "flag_parse.h"

right_pane::right_pane(unsigned int x) {
	pane = newwin(0, 0, 0, x / 2);
}

void right_pane::update(const screen_info &scr, const std::string &selected_filepath,
	       	size_t left_pane_size, const flag_parse &flags) {
	files.clear();
	file_content.clear();
	box(pane, 0, 0);
	if (left_pane_size != 0) {
		try {
			if (std::filesystem::is_directory(selected_filepath) && draw) {
				files = file_io::get_dir_files(selected_filepath, flags);
				std::sort(files.begin(), files.end());
				if (files.size() > scr.get_term_height()) {
					files.erase(files.begin() + scr.get_term_height(), files.end());
				}
			
			}
		} catch (const std::filesystem::filesystem_error &) {} //no permission to read contents
		if (file_io::file_contents_printable(selected_filepath)) {
			file_content = file_io::get_file_contents(selected_filepath, scr.get_term_height());
		}
		if (std::filesystem::is_directory(selected_filepath)) {
			draw_window_files(scr, draw, flags);
		}
		else {
			bool contents_printable = true;
			for (const auto &line : file_content) {
				if (line.find('\0') != std::string::npos) {
					contents_printable = false;
					break;
				}
			}
			draw_window_file_contents(scr, contents_printable);
		}
	}
	draw_window_title(selected_filepath, scr, draw);
	draw_window_info(scr, left_pane_size, selected_filepath, draw);
	wrefresh(pane);
}

void right_pane::draw_window_file_contents(const screen_info &scr, bool contents_printable) const {
	if (!contents_printable) {
		wattron(pane, COLOR_PAIR(5));
		mvwaddnstr(pane, 1, 1, "*** Binary contents unprintable ***", scr.get_x() / 2 - 2);
		wattroff(pane, COLOR_PAIR(5));
	}
	else {
		for (size_t i = 0; i < file_content.size(); i++) {
			mvwaddnstr(pane, i + 1, 1, file_content[i].c_str(), scr.get_x() / 2 - 2);
		}
	}
}
