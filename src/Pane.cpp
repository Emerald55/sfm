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

void Pane::draw_window_files(const Screen &scr, const FlagParse &flags, bool draw_curs) const {
	for (size_t i = 0; i < files.size(); i++) {
		std::string file = std::filesystem::path(files[i]).filename();
		std::string num_format = std::to_string(i + scr.get_page() - height + 3) + ".";
		if (!draw_curs) {
			num_format = std::to_string(i + 1) + "."; //lets right pane draw proper line numbers
		}
		wattron(pane, COLOR_PAIR(3));
		mvwaddstr(pane, i + 1, 1, num_format.c_str()); //draw line number
		wattroff(pane, COLOR_PAIR(3));
		try {
			if (i == scr.get_curs_y() && draw_curs && std::filesystem::is_directory(files[i])) {
				wattron(pane, COLOR_PAIR(6)); //highlight directory file cursor is on
			}
			else if (i == scr.get_curs_y() && draw_curs) {
				wattron(pane, COLOR_PAIR(1)); //hightlight file cursor is on
			}
			else if (std::filesystem::is_directory(files[i])) {
				wattron(pane, COLOR_PAIR(2)); //show file is a directory
			}
		}
		catch (const std::filesystem::filesystem_error &) {
			if (i == scr.get_curs_y() && draw_curs) {
				wattron(pane, COLOR_PAIR(7)); //highlight unknown file cursor is on
			}
			else {
				wattron(pane, COLOR_PAIR(8)); //show file is unknown
			}
		}
		try {
			if (flags.get_show_symbolic_links() && std::filesystem::is_symlink(files[i])) { //show symlink
				file += " -> ";
				file += std::filesystem::path(std::filesystem::read_symlink(files[i])).filename();
			}
		} catch (const std::filesystem::filesystem_error &) {} //ignore failed is_symlink()
		mvwaddnstr(pane, i + 1, 2 + num_format.size(), file.c_str(), width - 2 - num_format.size() - 1);
		wattroff(pane, COLOR_PAIR(1));
		wattroff(pane, COLOR_PAIR(2));
		wattroff(pane, COLOR_PAIR(3));
		wattroff(pane, COLOR_PAIR(7));
		wattroff(pane, COLOR_PAIR(8));
	}
}

void Pane::draw_window_info(const Screen &scr, unsigned int current_dir_size,
	       	const std::string &current_filepath, bool searching) const {
	std::string line_info;
	std::string page_info;
	if (current_dir_size > 0) {
		line_info = " Line: " + std::to_string(scr.get_curs_y() + 1 + scr.get_page() - height + 2)
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
	if (searching) {
		mvwaddstr(pane, height - 1, width - owner_and_perm_bits.size() - 
				line_info.size() - page_info.size() - 10, " S "); //search notification icon
	}
	if (current_dir_size > 0) {
		mvwaddstr(pane, height - 1, width - owner_and_perm_bits.size() - line_info.size() -
				page_info.size() - 8, owner_and_perm_bits.c_str()); //file owner and permission info
	}
	mvwaddstr(pane, height - 1, width - line_info.size() - page_info.size() - 5, line_info.c_str()); //line info
	mvwaddstr(pane, height - 1, width - page_info.size() - 2, page_info.c_str()); //page info
}

void Pane::draw_window_title(std::string path) const {
	std::string cut_down_path = std::filesystem::path(path).filename();
	try {
		if (std::filesystem::is_directory(path) && path != "/") { //append slash to show it's a directory
			path += "/";
			cut_down_path += "/";
		}
	}
	catch (const std::filesystem::filesystem_error &) {} //ignore failed is_directory()
	if (path.size() < width - 2) { //entire path
		mvwaddstr(pane, 0, width / 2 - (path.size() / 2), (" " + path + " ").c_str());
	}
	else if (cut_down_path.size() < width - 2) { //just display dir name/filename and not entire path
		mvwaddstr(pane, 0, width / 2 - (cut_down_path.size() / 2), (" " + cut_down_path + " ").c_str());
	}
	else { //path cant fit
		mvwaddstr(pane, 0, width / 2, (" ... "));
	}
}

Pane::~Pane() {
	delwin(pane);
}
