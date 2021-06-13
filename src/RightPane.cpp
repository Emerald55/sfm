#include <algorithm>
#include <filesystem>
#include <string>
#include "RightPane.h"
#include "FileIO.h"
#include "Screen.h"
#include "FlagParse.h"

RightPane::RightPane(unsigned int width, unsigned int height) {
	this->height = height;
	this->width = width / 2 + (width % 2);
	pane = newwin(height, this->width, 0, width / 2);
}

void RightPane::update(const Screen &scr, const std::string &selected_filepath,
	       	size_t left_pane_size, bool searching, const FlagParse &flags) {
	werase(pane);
	if (draw) {
		files.clear();
		file_content.clear();
		box(pane, 0, 0);
		if (left_pane_size != 0) {
			try {
				if (std::filesystem::is_directory(selected_filepath)) {
					files = FileIO::get_dir_files(selected_filepath, flags);
					std::sort(files.begin(), files.end());
					if (files.size() > scr.get_term_height()) {
						files.erase(files.begin() + scr.get_term_height(), files.end());
					}
				
				}
				if (FileIO::file_contents_printable(selected_filepath)) {
					file_content = FileIO::get_file_contents(selected_filepath, scr.get_term_height());
				}
				if (std::filesystem::is_directory(selected_filepath)) {
					draw_window_files(scr, flags);
				}
				else {
					bool contents_printable = true;
					for (const auto &line : file_content) {
						if (line.find('\0') != std::string::npos) { //if null byte assume binary file
							contents_printable = false;
							break;
						}
					}
					draw_window_file_contents(contents_printable);
				}
			} catch (const std::filesystem::filesystem_error &) {} //no permission to read contents
		}
		draw_window_title(selected_filepath);
		draw_window_info(scr, left_pane_size, selected_filepath, searching);
		wrefresh(pane);
	}
}

void RightPane::draw_window_file_contents(bool contents_printable) const {
	if (!contents_printable) {
		wattron(pane, COLOR_PAIR(5));
		mvwaddnstr(pane, 1, 1, "*** Binary contents unprintable ***", width - 2);
		wattroff(pane, COLOR_PAIR(5));
	}
	else {
		for (size_t i = 0; i < file_content.size(); i++) {
			mvwaddnstr(pane, i + 1, 1, file_content[i].c_str(), width - 2);
		}
	}
}
