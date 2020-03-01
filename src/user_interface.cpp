#include <ncurses.h>
#include <chrono>
#include <string>
#include <thread>
#include <filesystem>
#include <vector>
#include "file_io.h"
#include "user_interface.h"

void user_interface::draw_window_files(const std::string &path, const std::vector<std::string> &files, WINDOW *win, 
		const int &argc, char* argv[], const bool &draw_curs) {
	for (size_t i = 0; i < files.size(); i++) {
		std::string file = file_io::path_to_filename(files[i]);
		std::string num_format = std::to_string(i + page - scr_y + 3) + ".";
		if (!draw_curs) {
			num_format = std::to_string(i + 1) + ".";
		}
		wattron(win, COLOR_PAIR(3));
		mvwaddstr(win, i + 1, 1, num_format.c_str());
		wattroff(win, COLOR_PAIR(3));
		if (std::filesystem::is_directory(files[i])) {
			wattron(win, COLOR_PAIR(2));
		}
		if (i == static_cast<unsigned>(curs_y) && draw_curs) { //highlight file where cursor is
			wattron(win, COLOR_PAIR(1));
		}
		if (std::filesystem::is_symlink(files[i]) && check_for_flag(argc, argv, "-s")) {
			file += " -> ";
			file += file_io::path_to_filename(std::filesystem::read_symlink(files[i]));
		}
		const unsigned int current_scr_size = draw_selected_path ? scr_x / 2 - 2 : scr_x;
		mvwaddnstr(win, i + 1, 2 + num_format.size(), file.c_str(), current_scr_size - num_format.size() - 1);
		wattroff(win, COLOR_PAIR(1));
		wattroff(win, COLOR_PAIR(2));
	}
	box(win, 0, 0);
	draw_window_title(path, win);
}
void user_interface::draw_window_file_contents(const std::string &path, WINDOW *win, 
		std::vector<std::string> file_contents) {
	for (size_t i = 0; i < file_contents.size(); i++) {
		if (i > term_height - 1) { break; }
		mvwaddnstr(win, i + 1, 1, file_contents[i].c_str(), scr_x / 2 - 2);
	}
	box(win, 0, 0);
	draw_window_title(path, win);
}
void user_interface::draw_info(WINDOW *win, const unsigned int &page,
	       	const unsigned int &current_dir_size) {
	std::string line_info = " Line: " + std::to_string(curs_y + 1 + page - scr_y + 2)
		+ "/" + std::to_string(current_dir_size) + " ";
	std::string page_info = " Page: " + std::to_string(page / term_height) +
		"/" + std::to_string(current_dir_size / term_height + 1) +  " ";
	const unsigned int offset = draw_selected_path ? scr_x / 2 : scr_x;
	mvwaddstr(win, scr_y - 1, offset - line_info.size() - page_info.size() - 5,
		       	line_info.c_str());
	mvwaddstr(win, scr_y - 1, offset - page_info.size() - 2, page_info.c_str());
}
std::string user_interface::input(const char* text, const unsigned int &win_width,
	       	const unsigned int &color_type) {
	curs_set(1);
	WINDOW *input_win = newwin(1, win_width, scr_y - 1, 2);
	wattron(input_win, COLOR_PAIR(color_type));
	mvwaddstr(input_win, 0, 0, text);
	wattroff(input_win, COLOR_PAIR(color_type));
	echo();
	char user_input[win_width];
	wgetnstr(input_win, user_input, win_width);
	noecho();
	wrefresh(input_win);
	curs_set(0);
	delwin(input_win);
	return std::string(user_input);
}
void user_interface::alert_box(const char* text, const unsigned int &win_width,
	       	const unsigned int &sleep_time, const unsigned int &alert_color) {
	WINDOW *alert_win = newwin(1, win_width, scr_y - 1, 2);
	wattron(alert_win, COLOR_PAIR(alert_color)); //5
	mvwaddstr(alert_win, 0, 0, text);
	wattroff(alert_win, COLOR_PAIR(alert_color));
	wrefresh(alert_win);
	std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
	delwin(alert_win);
}
void user_interface::draw_window_title(const std::string &path, WINDOW *win) {
	const unsigned int scr_center = draw_selected_path ? scr_x / 4 : scr_x / 2;
	const unsigned int win_width = draw_selected_path ? scr_x / 2 - 2 : scr_x - 2;
	std::string cut_down_path = std::filesystem::is_directory(path) ? path.substr(0, path.size() - 1) : path;
	cut_down_path = cut_down_path.substr(cut_down_path.find_last_of("/\\") + 1);
	if (path.size() < win_width) {
		mvwaddstr(win, 0, scr_center - (path.size() / 2), (" " + path + " ").c_str());
	}
	else if (cut_down_path.size() < win_width) { //just display dir name/filename and not entire path
		mvwaddstr(win, 0, scr_center - (cut_down_path.size() / 2), (" " + cut_down_path + " ").c_str());
	}
	else {
		mvwaddstr(win, 0, scr_center, (" ... "));
	}
}
