#include <ncurses.h>
#include <chrono>
#include <string>
#include <thread>
#include <filesystem>
#include <vector>
#include "file_io.h"
#include "user_interface.h"

user_interface::user_interface() {
	getmaxyx(stdscr, scr_y, scr_x);
	start_color();
	init_pair(1, COLOR_WHITE, COLOR_RED); //cursor
	init_pair(2, COLOR_YELLOW, COLOR_BLACK); //directories
	init_pair(3, COLOR_CYAN, COLOR_BLACK); //line numbers
	init_pair(4, COLOR_GREEN, COLOR_BLACK); //green input box
	init_pair(5, COLOR_RED, COLOR_BLACK); //red input box and unprintable warning
	cbreak();
	noecho();
	curs_set(0);
	keypad(stdscr, true);
	left_pane = newwin(scr_y, scr_x, 0, 0);
	right_pane = newwin(0, 0, 0, scr_x / 2);
	term_height = scr_y - 2;
	page = term_height;
}

void user_interface::draw_window_files(const std::vector<std::string> &files, WINDOW *win, 
		bool show_symbolic_links, bool draw_curs) {
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
		if (show_symbolic_links && std::filesystem::is_symlink(files[i])) {
			file += " -> ";
			file += file_io::path_to_filename(std::filesystem::read_symlink(files[i]));
		}
		const unsigned int current_scr_size = draw_right_pane ? scr_x / 2 - 2 : scr_x - 2;
		mvwaddnstr(win, i + 1, 2 + num_format.size(), file.c_str(), current_scr_size - num_format.size() - 1);
		wattroff(win, COLOR_PAIR(1));
		wattroff(win, COLOR_PAIR(2));
	}
}

void user_interface::draw_window_file_contents(WINDOW *win, 
		std::vector<std::string> file_contents, bool contents_printable) {
	if (!contents_printable) {
		wattron(win, COLOR_PAIR(5));
		mvwaddnstr(win, 1, 1, "*** Contents unprintable ***", scr_x / 2 - 2);
		wattroff(win, COLOR_PAIR(5));
	}
	else {
		for (size_t i = 0; i < file_contents.size(); i++) {
			if (i > term_height - 1) { break; }
			mvwaddnstr(win, i + 1, 1, file_contents[i].c_str(), scr_x / 2 - 2);
		}
	}
}

void user_interface::draw_info(WINDOW* win, unsigned int current_dir_size, const std::string &current_filepath) {
	std::string line_info;
	std::string page_info;
	if (current_dir_size > 0) {
		line_info = " Line: " + std::to_string(curs_y + 1 + page - scr_y + 2)
			+ "/" + std::to_string(current_dir_size) + " ";
		page_info = " Page: " + std::to_string(page / term_height) +
			"/" + std::to_string(current_dir_size / term_height + 1) +  " ";
	}
	else {
		line_info = " Line: 0/0 ";
		page_info = " Page: 0/0 ";
	}
	const unsigned int offset = draw_right_pane ? scr_x / 2 : scr_x;
	const std::string owner_and_perm_bits = file_io::get_permbits(current_filepath);
	mvwaddstr(win, scr_y - 1, offset - owner_and_perm_bits.size() - line_info.size() -
		       	page_info.size() - 8, owner_and_perm_bits.c_str());
	mvwaddstr(win, scr_y - 1, offset - line_info.size() - page_info.size() - 5, line_info.c_str());
	mvwaddstr(win, scr_y - 1, offset - page_info.size() - 2, page_info.c_str());
}

std::string user_interface::input(const std::string &text, unsigned int color_type, 
		unsigned int win_width) {
	WINDOW *input_text_win = newwin(1, text.size(), scr_y - 1, 2);
	wattron(input_text_win, COLOR_PAIR(color_type));
	waddstr(input_text_win, text.c_str());
	wattroff(input_text_win, COLOR_PAIR(color_type));
	wrefresh(input_text_win);
	WINDOW *input_win = newwin(1, win_width, scr_y - 1, 2 + text.size());
	curs_set(1);
	scrollok(input_win, 1);
	echo();
	nocbreak();
	std::string user_input;
	for (;;) {
		const char input_char = wgetch(input_win);
		if (input_char == ERR || input_char == '\n') {
			break;
		}
		user_input += input_char;
	}
	cbreak();
	noecho();
	wrefresh(input_win);
	curs_set(0);
	delwin(input_win);
	delwin(input_text_win);
	return user_input;
}

void user_interface::alert_box(const char* text, unsigned int win_width, 
		unsigned int sleep_time, unsigned int alert_color) {
	WINDOW *alert_win = newwin(1, win_width, scr_y - 1, 2);
	wattron(alert_win, COLOR_PAIR(alert_color));
	mvwaddstr(alert_win, 0, 0, text);
	wattroff(alert_win, COLOR_PAIR(alert_color));
	wrefresh(alert_win);
	std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
	delwin(alert_win);
}

void user_interface::draw_window_title(std::string path, WINDOW *win) {
	const unsigned int scr_center = draw_right_pane ? scr_x / 4 : scr_x / 2;
	const unsigned int win_width = draw_right_pane ? scr_x / 2 - 2 : scr_x - 2;
	std::string cut_down_path = file_io::path_to_filename(path);
	if (std::filesystem::is_directory(path) && path != "/") {
		path += "/";
		cut_down_path += "/";
	}
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

void user_interface::check_resize() {
	unsigned int check_scr_y, check_scr_x;
	getmaxyx(stdscr, check_scr_y, check_scr_x);
	if ((check_scr_y != scr_y || check_scr_x != scr_x) &&
		       	check_scr_y > 2 && check_scr_x > 2) { //check for screen resize
		scr_y = check_scr_y;
		scr_x = check_scr_x;
		wresize(left_pane, scr_y, scr_x / 2);
		if (!draw_right_pane) {
			wresize(left_pane, scr_y, scr_x);
		}
		wresize(right_pane, scr_y, scr_x / 2);
		mvwin(right_pane, 0, scr_x / 2);
		term_height = scr_y - 2;
		page = term_height;
	}
}
