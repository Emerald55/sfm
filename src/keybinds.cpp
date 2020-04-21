#include <ncurses.h>
#include <iostream>
#include <unistd.h>
#include <filesystem>
#include <vector>
#include <cmath>
#include <sys/wait.h>
#include "input.h"
#include "file_io.h"
#include "keybinds.h"

bool keybinds::quit(const screen_info &scr) const {
	std::string user_input = input::input_box(" Quit? [Y/n]: ", 5, scr, 2);
	if (user_input.empty() || user_input == "y" || user_input == "Y") {
		return false;
	}
	return true;
}

void keybinds::move_left(screen_info &scr) {
	scr.curs_y = 0;
	scr.page = scr.term_height;
	search_str = "";
	chdir("..");
}

void keybinds::move_right(screen_info &scr,
	       	const std::string &selected_filepath) {
	if (std::filesystem::is_directory(selected_filepath)) {
		int err = chdir(selected_filepath.c_str());
		if (err == -1) {
			input::alert_box(" Invalid Permission ", 20, 750, 5, scr);
		}
		else {
			scr.page = scr.term_height;
			scr.curs_y = 0;
			search_str = "";
		}
	}
}

void keybinds::move_up(screen_info &scr) const {
	if (scr.curs_y > 0) {
		scr.curs_y--;
	}
	else if (scr.page != scr.term_height) {
		scr.curs_y = scr.term_height - 1;
		scr.page -= scr.term_height;
	}
}

void keybinds::move_down(screen_info &scr, size_t left_pane_size_currently, 
		size_t left_pane_size) const {
	if (left_pane_size_currently > 0) {
		if (scr.curs_y < left_pane_size_currently - 1) {
			scr.curs_y++;
		}
		else if ((scr.curs_y + 1 + scr.page - scr.y + 2) != left_pane_size) {
			scr.page += scr.term_height;
			scr.curs_y = 0;
		}
	}
}

void keybinds::jump_to_bottom(screen_info &scr, size_t left_pane_size_currently) const {
	if (left_pane_size_currently > 0) {
		scr.curs_y = left_pane_size_currently - 1;
	}
}

void keybinds::up_page(screen_info &scr) const {
	if (scr.page != scr.term_height) {
		scr.page -= scr.term_height;
		scr.curs_y = 0;
	}
}

void keybinds::down_page(screen_info &scr, size_t left_pane_size) const {
	if (left_pane_size > scr.page) {
		scr.page += scr.term_height;
		scr.curs_y = 0;
	}
}

void keybinds::jump_to_line(screen_info &scr, size_t left_pane_size) const {
	std::string user_input = input::input_box(" Jump To: ", 4, scr, 10);
	try {
		if (!user_input.empty() && std::stoul(user_input) > 0 && std::stoul(user_input) <= 
				left_pane_size) {
			unsigned int cursor_location = std::stoi(user_input) % scr.term_height;
			if (std::stoul(user_input) == scr.term_height || cursor_location == 0) {
				cursor_location = scr.term_height;
			}
			scr.page = static_cast<unsigned int>(std::ceil(static_cast<double>(std::stoi(user_input))
						/ static_cast<double>(scr.term_height))) * scr.term_height;
			scr.curs_y = cursor_location - 1;
		}
		
	}
	catch (const std::invalid_argument &) {
		input::alert_box(" Not A Number ", 14, 750, 5, scr);
	}
	catch (const std::out_of_range &) {
		input::alert_box(" Too large of a number ", 23, 750, 5, scr);
	}
}

void keybinds::edit_text(const std::string &selected_filepath) const {
	def_prog_mode();
	endwin();
	pid_t pid = fork();
	if (pid == 0) {
		try {
			const std::string editor = std::getenv("EDITOR");
			execl(editor.c_str(), file_io::path_to_filename(editor).c_str(),
					selected_filepath.c_str(), (char*)0);
		}
		catch (const std::exception &e) {
			std::cerr << "\"$EDITOR\" environmental variable not set.\n";
		}
		exit(1);
	}
	int ret;
	int status_child;
	do {
		ret = waitpid(pid, &status_child, 0);
	} while (ret != pid || !WIFEXITED(status_child));
	reset_prog_mode();
}

void keybinds::pager(const std::string &selected_filepath) const {
	def_prog_mode();
	endwin();
	pid_t pid = fork();
	if (pid == 0) {
		try {
			const std::string pager = std::getenv("PAGER");
			execl(pager.c_str(), file_io::path_to_filename(pager).c_str(), 
				       selected_filepath.c_str(), (char*)0);
		}
		catch (const std::exception &e) {
			std::cerr << "\"$PAGER\" environmental variable not set.\n";
		}
		exit(1);
	}
	int ret;
	int status_child;
	do {
		ret = waitpid(pid, &status_child, 0);
	} while (ret != pid || !WIFEXITED(status_child));
	reset_prog_mode();
}

void keybinds::spawn_shell() const {
	def_prog_mode();
	endwin();
	pid_t pid = fork();
	if (pid == 0) {
		try {
			const std::string shell = std::getenv("SHELL");
			execl(shell.c_str(), file_io::path_to_filename(shell).c_str(), (char*)0);
		}
		catch (const std::exception &e) {
			std::cerr << "\"$SHELL\" environmental variable not set.\n";
		}
		exit(1);
	}
	int ret;
	int status_child;
	do {
		ret = waitpid(pid, &status_child, 0);
	} while (ret != pid || !WIFEXITED(status_child));
	reset_prog_mode();
}

void keybinds::xdg_open(const std::string &selected_filepath) const {
	def_prog_mode();
	endwin();
	pid_t pid = fork();
	if (pid == 0) {
		execl("/usr/bin/xdg-open", "xdg-open",
				selected_filepath.c_str(), (char*)0);
		exit(1);
	}
	int ret;
	int status_child;
	do {
		ret = waitpid(pid, &status_child, 0);
	} while (ret != pid || !WIFEXITED(status_child));
	reset_prog_mode();
}

void keybinds::remove(const screen_info &scr, const std::string &selected_filepath) const {
	std::string user_input = input::input_box(" Delete File/Directory? [y/N]: ", 5, scr, 2);
	try {
		if (std::filesystem::exists(selected_filepath) &&
				(user_input == "Y" || user_input == "y")) {
			std::filesystem::remove_all(selected_filepath);
		}
	}
	catch (const std::filesystem::filesystem_error &) {
		input::alert_box(" Delete Failed ", 15, 750, 5, scr);
	}
}

void keybinds::rename(const screen_info &scr, const std::string &selected_filepath) const {
	std::string user_input = input::input_box(" Rename: ", 4, scr);
	if (!user_input.empty() && std::filesystem::exists(selected_filepath)) {
		try {
			std::filesystem::rename(selected_filepath, user_input);
		}
		catch (const std::filesystem::filesystem_error &) {
			input::alert_box(" Invalid Filename ", 18, 750, 5, scr);
		}
	}
}

void keybinds::copy(const screen_info &scr, const std::string &selected_filepath) {
	copy_path = selected_filepath;
	cut_path = false;
	input::alert_box((" Copied: " + file_io::path_to_filename(selected_filepath) + " ").c_str(),
			10 + file_io::path_to_filename(selected_filepath).size(), 500, 4, scr);
}

void keybinds::cut(const screen_info &scr, const std::string &selected_filepath) {
	std::string user_input = input::input_box(" Cut File/Directory? [y/N]: ", 5, scr, 2);
	if (std::filesystem::exists(selected_filepath) &&
			(user_input == "y" || user_input == "Y")) {
		copy_path = selected_filepath;
		cut_path = true;
	}
}

void keybinds::paste(screen_info &scr, const std::string &current_path) {
	if (std::filesystem::exists(copy_path)) {
		try {
			std::filesystem::copy(copy_path, current_path + "/" + file_io::path_to_filename(copy_path),
					std::filesystem::copy_options::recursive);
			if (cut_path) {
				input::alert_box((" Cut: " + file_io::path_to_filename(copy_path) + " ").c_str(),
						7 + file_io::path_to_filename(copy_path).size(), 500, 4, scr);
				std::filesystem::remove_all(copy_path);
				scr.curs_y = 0;
				scr.page = scr.term_height;
				cut_path = false;
			}
			else {
				input::alert_box((" Pasted: " + file_io::path_to_filename(copy_path) + " ").c_str(),
						10 + file_io::path_to_filename(copy_path).size(), 500, 4, scr);
			}
		}
		catch (std::exception& e) {
			if (cut_path) {
				input::alert_box(" Cut Failed ", 12, 750, 5, scr);
				cut_path = false;
			}
			else {
				input::alert_box(" Paste Failed ", 14, 750, 5, scr);
			}
		}
	}
}

void keybinds::search(screen_info &scr) {
	if (search_str.empty()) {
		std::string user_input = input::input_box(" Search: ", 4, scr);
		if (!user_input.empty()) {
			search_str = user_input;
			scr.curs_y = 0;
			scr.page = scr.term_height;
		}
	}
	else {
		search_str = "";
		scr.curs_y = 0;
		scr.page = scr.term_height;
	}
}

void keybinds::screen_change(const screen_info &scr, WINDOW* left_pane, bool &draw_right_pane) const {
	const unsigned int win_resize_width = draw_right_pane ? scr.x : scr.x / 2;
	draw_right_pane = !draw_right_pane;
	wresize(left_pane, scr.y, win_resize_width);
}

void keybinds::help(const screen_info &scr) const {
	const unsigned int help_win_y = 27;
	const unsigned int help_win_x = 80;
	WINDOW *help_win = newwin(help_win_y, help_win_x, scr.y / 2 - help_win_y / 2,
			scr.x / 2 - help_win_x / 2);
	mvwaddstr(help_win, 1, help_win_x / 2 - 5, "Keybinds:");
	mvwaddstr(help_win, 3, 1, "\"q\" - Quit or exit");
	mvwaddstr(help_win, 4, 1, "\"w\" or \"k\" or UP ARROW - Move cursor up");
	mvwaddstr(help_win, 5, 1, "\"s\" or \"j\" or DOWN ARROW - Move cursor down");
	mvwaddstr(help_win, 6, 1, "\"a\" or \"h\" or LEFT ARROW - Move back a directory");
	mvwaddstr(help_win, 7, 1, "\"d\" or \"l\" or RIGHT ARROW - Move into a directory");
	mvwaddstr(help_win, 8, 1, "\";\" - Split screen toggle");
	mvwaddstr(help_win, 9, 1, "SPACEBAR - Jump to specific line number (leave empty to cancel)");
	mvwaddstr(help_win, 10, 1, "\"=\" - Go forward a page");
	mvwaddstr(help_win, 11, 1, "\"-\" - Go back a page");
	mvwaddstr(help_win, 12, 1, "\"m\" - Jump to top of current page");
	mvwaddstr(help_win, 13, 1, "\"n\" - Jump to bottom of current page");
	mvwaddstr(help_win, 14, 1, "\"i\" or ENTER - Open file or folder with xdg-open");
	mvwaddstr(help_win, 15, 1, "\"y\" - Open file or folder with pager");
	mvwaddstr(help_win, 16, 1, "\"u\" - Spawn interactive shell");
	mvwaddstr(help_win, 17, 1, "\"v\" - Edit file or folder with default editor");
	mvwaddstr(help_win, 18, 1, "\"b\" - Search for file or folder (press again to stop)");
	mvwaddstr(help_win, 19, 1, "\"e\" - Rename file or folder (leave empty to cancel)");
	mvwaddstr(help_win, 20, 1, "\"g\" - Delete file or folder");
	mvwaddstr(help_win, 21, 1, "\"c\" - Copy a file or folder");
	mvwaddstr(help_win, 22, 1, "\"p\" - Paste a file or folder");
	mvwaddstr(help_win, 23, 1, "\"x\" - Cut a file or folder (deletes after paste rather than before)");
	mvwaddstr(help_win, 25, 1, "Press any key to exit help menu...");
	box(help_win, 0, 0);
	wrefresh(help_win);
	timeout(-1);
	getch();
	delwin(help_win);
	timeout(scr.update_speed);
}
