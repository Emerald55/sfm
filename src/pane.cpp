#include <ncurses.h>
#include <string>
#include <filesystem>
#include <vector>
#include <cmath>
#include "pane.h"
#include "file_io.h"
#include "screen_info.h"
#include "flag_parse.h"

void pane::draw_window_files(const screen_info &scr, bool draw_right_pane, 
		const flag_parse &flags, bool draw_curs) const {
	for (size_t i = 0; i < files.size(); i++) {
		std::string file = file_io::path_to_filename(files[i]);
		std::string num_format = std::to_string(i + scr.get_page() - scr.get_y() + 3) + ".";
		if (!draw_curs) {
			num_format = std::to_string(i + 1) + "."; //lets right pane draw proper numbers
		}
		wattron(pane, COLOR_PAIR(3));
		mvwaddstr(pane, i + 1, 1, num_format.c_str());
		wattroff(pane, COLOR_PAIR(3));
		if (i == scr.get_curs_y() && draw_curs && std::filesystem::is_directory(files[i])) { //highlight file where cursor is
			wattron(pane, COLOR_PAIR(6));
		}
		else if (i == scr.get_curs_y() && draw_curs) {
			wattron(pane, COLOR_PAIR(1));
		}
		else if (std::filesystem::is_directory(files[i])) {
			wattron(pane, COLOR_PAIR(2));
		}
		if (flags.show_symbolic_links && std::filesystem::is_symlink(files[i])) {
			file += " -> ";
			file += file_io::path_to_filename(std::filesystem::read_symlink(files[i]));
		}
		const unsigned int current_scr_size = draw_right_pane ? scr.get_x() / 2 - 2 : scr.get_x() - 2;
		mvwaddnstr(pane, i + 1, 2 + num_format.size(), file.c_str(), current_scr_size - num_format.size() - 1);
		wattroff(pane, COLOR_PAIR(1));
		wattroff(pane, COLOR_PAIR(2));
		wattroff(pane, COLOR_PAIR(3));
	}
}

void pane::draw_window_info(const screen_info &scr, unsigned int current_dir_size,
	       	const std::string &current_filepath, bool draw_right_pane) const {
	std::string line_info;
	std::string page_info;
	if (current_dir_size > 0) {
		line_info = " Line: " + std::to_string(scr.get_curs_y() + 1 + scr.get_page() - scr.get_y() + 2)
			+ "/" + std::to_string(current_dir_size) + " ";
		const double max_pages = std::ceil(static_cast<double>(current_dir_size) /
			       	static_cast<double>(scr.get_term_height()));
		page_info = " Page: " + std::to_string(scr.get_page() / scr.get_term_height()) +
			"/" + std::to_string(static_cast<unsigned int>(max_pages)) +  " ";
	}
	else {
		line_info = " Line: 0/0 ";
		page_info = " Page: 0/0 ";
	}
	const unsigned int offset = draw_right_pane ? scr.get_x() / 2 : scr.get_x();
	const std::string owner_and_perm_bits = file_io::get_permbits(current_filepath);
	if (current_dir_size > 0) {
		mvwaddstr(pane, scr.get_y() - 1, offset - owner_and_perm_bits.size() - line_info.size() -
				page_info.size() - 8, owner_and_perm_bits.c_str());
	}
	mvwaddstr(pane, scr.get_y() - 1, offset - line_info.size() - page_info.size() - 5, line_info.c_str());
	mvwaddstr(pane, scr.get_y() - 1, offset - page_info.size() - 2, page_info.c_str());
}

void pane::draw_window_title(std::string path, const screen_info &scr, bool draw_right_pane) const {
	const unsigned int scr_center = draw_right_pane ? scr.get_x() / 4 : scr.get_x() / 2;
	const unsigned int win_width = draw_right_pane ? scr.get_x() / 2 - 2 : scr.get_x() - 2;
	std::string cut_down_path = file_io::path_to_filename(path);
	if (std::filesystem::is_directory(path) && path != "/") {
		path += "/";
		cut_down_path += "/";
	}
	if (path.size() < win_width) {
		mvwaddstr(pane, 0, scr_center - (path.size() / 2), (" " + path + " ").c_str());
	}
	else if (cut_down_path.size() < win_width) { //just display dir name/filename and not entire path
		mvwaddstr(pane, 0, scr_center - (cut_down_path.size() / 2), (" " + cut_down_path + " ").c_str());
	}
	else {
		mvwaddstr(pane, 0, scr_center, (" ... "));
	}
}

pane::~pane() {
	delwin(pane);
}
