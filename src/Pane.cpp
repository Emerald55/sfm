#include <ncurses.h>
#include <string>
#include <filesystem>
#include <vector>
#include <cmath>
#include "Pane.h"
#include "FileIO.h"
#include "Screen.h"
#include "FlagParse.h"

void Pane::resize(unsigned int width, unsigned int height) {
	this->width = width;
	this->height = height;
	wresize(pane, height, width);
}

void Pane::reposition(unsigned int x, unsigned int y) {
	mvwin(pane, y, x);
}

void Pane::draw_window_files(const Screen &scr, bool draw_right_pane, 
		const FlagParse &flags, bool draw_curs) const {
	for (size_t i = 0; i < files.size(); i++) {
		std::string file = FileIO::path_to_filename(files[i]);
		std::string num_format = std::to_string(i + scr.get_page() - height + 3) + ".";
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
		if (flags.get_show_symbolic_links() && std::filesystem::is_symlink(files[i])) {
			file += " -> ";
			file += FileIO::path_to_filename(std::filesystem::read_symlink(files[i]));
		}
		const unsigned int current_scr_size = draw_right_pane ? scr.get_x() / 2 - 2 : scr.get_x() - 2;
		mvwaddnstr(pane, i + 1, 2 + num_format.size(), file.c_str(), current_scr_size - num_format.size() - 1);
		wattroff(pane, COLOR_PAIR(1));
		wattroff(pane, COLOR_PAIR(2));
		wattroff(pane, COLOR_PAIR(3));
	}
}

void Pane::draw_window_info(const Screen &scr, unsigned int current_dir_size,
	       	const std::string &current_filepath) const {
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
	const std::string owner_and_perm_bits = FileIO::get_permbits(current_filepath);
	if (current_dir_size > 0) {
		mvwaddstr(pane, scr.get_y() - 1, width - owner_and_perm_bits.size() - line_info.size() -
				page_info.size() - 8, owner_and_perm_bits.c_str());
	}
	mvwaddstr(pane, scr.get_y() - 1, width - line_info.size() - page_info.size() - 5, line_info.c_str());
	mvwaddstr(pane, scr.get_y() - 1, width - page_info.size() - 2, page_info.c_str());
}

void Pane::draw_window_title(std::string path) const {
	std::string cut_down_path = FileIO::path_to_filename(path);
	if (std::filesystem::is_directory(path) && path != "/") {
		path += "/";
		cut_down_path += "/";
	}
	if (path.size() < width - 2) {
		mvwaddstr(pane, 0, width / 2 - (path.size() / 2), (" " + path + " ").c_str());
	}
	else if (cut_down_path.size() < width - 2) { //just display dir name/filename and not entire path
		mvwaddstr(pane, 0, width / 2 - (cut_down_path.size() / 2), (" " + cut_down_path + " ").c_str());
	}
	else {
		mvwaddstr(pane, 0, width / 2, (" ... "));
	}
}

Pane::~Pane() {
	delwin(pane);
}
