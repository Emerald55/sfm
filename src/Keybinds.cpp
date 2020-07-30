#include <ncurses.h>
#include <iostream>
#include <unistd.h>
#include <filesystem>
#include <vector>
#include <cmath>
#include <sys/wait.h>
#include "Input.h"
#include "FileIO.h"
#include "Keybinds.h"

bool Keybinds::quit(const Screen &scr) const {
	std::string user_input = Input::input_box(" Quit? [Y/n]: ", 5, scr, 2);
	if (user_input.empty() || user_input == "y" || user_input == "Y") {
		return false;
	}
	return true;
}

void Keybinds::move_left(Screen &scr) {
	scr.reset_to_first_page();
	search_str = "";
	chdir("..");
}

void Keybinds::move_right(Screen &scr,
	       	const std::string &selected_filepath) {
	if (std::filesystem::is_directory(selected_filepath)) {
		int err = chdir(selected_filepath.c_str());
		if (err == -1) {
			Input::alert_box(" Invalid Permission ", 20, 750, 5, scr);
		}
		else {
			scr.reset_to_first_page();
			search_str = "";
		}
	}
}

void Keybinds::move_up(Screen &scr) const {
	if (scr.get_curs_y() > 0) {
		scr.set_curs_y(scr.get_curs_y() - 1);
	}
	else if (scr.get_page() != scr.get_term_height()) {
		scr.set_curs_y(scr.get_term_height() - 1);
		scr.set_page(scr.get_page() - scr.get_term_height());
	}
}

void Keybinds::move_down(Screen &scr, size_t left_pane_size_currently, 
		size_t left_pane_size) const {
	if (left_pane_size_currently > 0) {
		if (scr.get_curs_y() < left_pane_size_currently - 1) {
			scr.set_curs_y(scr.get_curs_y() + 1);
		}
		else if ((scr.get_curs_y() + 1 + scr.get_page() - scr.get_y() + 2) != left_pane_size) {
			scr.set_page(scr.get_page() + scr.get_term_height());
			scr.set_curs_y(0);
		}
	}
}

void Keybinds::jump_to_bottom(Screen &scr, size_t left_pane_size_currently) const {
	if (left_pane_size_currently > 0) {
		scr.set_curs_y(left_pane_size_currently - 1);
	}
}

void Keybinds::up_page(Screen &scr) const {
	if (scr.get_page() != scr.get_term_height()) {
		scr.set_page(scr.get_page() - scr.get_term_height());
		scr.set_curs_y(0);
	}
}

void Keybinds::down_page(Screen &scr, size_t left_pane_size) const {
	if (left_pane_size > scr.get_page()) {
		scr.set_page(scr.get_page() + scr.get_term_height());
		scr.set_curs_y(0);
	}
}

void Keybinds::jump_to_line(Screen &scr, size_t left_pane_size) const {
	std::string user_input = Input::input_box(" Jump To: ", 4, scr, 10);
	try {
		if (!user_input.empty() && std::stoul(user_input) > 0 && std::stoul(user_input) <= 
				left_pane_size) {
			unsigned int cursor_location = std::stoi(user_input) % scr.get_term_height();
			if (std::stoul(user_input) == scr.get_term_height() || cursor_location == 0) {
				cursor_location = scr.get_term_height();
			}
			scr.set_page(static_cast<unsigned int>(std::ceil(static_cast<double>(std::stoi(user_input))
						/ static_cast<double>(scr.get_term_height()))) * scr.get_term_height());
			scr.set_curs_y(cursor_location - 1);
		}
		
	}
	catch (const std::invalid_argument &) {
		Input::alert_box(" Not A Number ", 14, 750, 5, scr);
	}
	catch (const std::out_of_range &) {
		Input::alert_box(" Too large of a number ", 23, 750, 5, scr);
	}
}

void Keybinds::edit_text(const std::string &selected_filepath) const {
	def_prog_mode();
	endwin();
	pid_t pid = fork();
	if (pid == 0) {
		try {
			const std::string editor = std::getenv("EDITOR");
			execl(editor.c_str(), FileIO::path_to_filename(editor).c_str(),
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

void Keybinds::pager(const std::string &selected_filepath) const {
	def_prog_mode();
	endwin();
	pid_t pid = fork();
	if (pid == 0) {
		try {
			const std::string pager = std::getenv("PAGER");
			execl(pager.c_str(), FileIO::path_to_filename(pager).c_str(), 
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

void Keybinds::spawn_shell() const {
	def_prog_mode();
	endwin();
	pid_t pid = fork();
	if (pid == 0) {
		try {
			const std::string shell = std::getenv("SHELL");
			execl(shell.c_str(), FileIO::path_to_filename(shell).c_str(), (char*)0);
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

void Keybinds::xdg_open(const std::string &selected_filepath) const {
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

void Keybinds::remove(const Screen &scr, const std::string &selected_filepath) const {
	std::string user_input = Input::input_box(" Delete File/Directory? [y/N]: ", 5, scr, 2);
	try {
		if (std::filesystem::exists(selected_filepath) &&
				(user_input == "Y" || user_input == "y")) {
			std::filesystem::remove_all(selected_filepath);
		}
	}
	catch (const std::filesystem::filesystem_error &) {
		Input::alert_box(" Delete Failed ", 15, 750, 5, scr);
	}
}

void Keybinds::rename(const Screen &scr, const std::string &selected_filepath) const {
	std::string user_input = Input::input_box(" Rename: ", 4, scr);
	if (!user_input.empty() && std::filesystem::exists(selected_filepath)) {
		try {
			std::filesystem::rename(selected_filepath, user_input);
		}
		catch (const std::filesystem::filesystem_error &) {
			Input::alert_box(" Invalid Filename ", 18, 750, 5, scr);
		}
	}
}

void Keybinds::copy(const Screen &scr, const std::string &selected_filepath) {
	copy_path = selected_filepath;
	cut_path = false;
	Input::alert_box((" Copied: " + FileIO::path_to_filename(selected_filepath) + " ").c_str(),
			10 + FileIO::path_to_filename(selected_filepath).size(), 500, 4, scr);
}

void Keybinds::cut(const Screen &scr, const std::string &selected_filepath) {
	std::string user_input = Input::input_box(" Cut File/Directory? [y/N]: ", 5, scr, 2);
	if (std::filesystem::exists(selected_filepath) &&
			(user_input == "y" || user_input == "Y")) {
		copy_path = selected_filepath;
		cut_path = true;
	}
}

void Keybinds::paste(Screen &scr, const std::string &current_path) {
	if (std::filesystem::exists(copy_path)) {
		try {
			std::filesystem::copy(copy_path, current_path + "/" + FileIO::path_to_filename(copy_path),
					std::filesystem::copy_options::recursive);
			if (cut_path) {
				Input::alert_box((" Cut: " + FileIO::path_to_filename(copy_path) + " ").c_str(),
						7 + FileIO::path_to_filename(copy_path).size(), 500, 4, scr);
				std::filesystem::remove_all(copy_path);
				scr.reset_to_first_page();
				cut_path = false;
			}
			else {
				Input::alert_box((" Pasted: " + FileIO::path_to_filename(copy_path) + " ").c_str(),
						10 + FileIO::path_to_filename(copy_path).size(), 500, 4, scr);
			}
		}
		catch (std::exception& e) {
			if (cut_path) {
				Input::alert_box(" Cut Failed ", 12, 750, 5, scr);
				cut_path = false;
			}
			else {
				Input::alert_box(" Paste Failed ", 14, 750, 5, scr);
			}
		}
	}
}

void Keybinds::search(Screen &scr) {
	if (search_str.empty()) {
		std::string user_input = Input::input_box(" Search: ", 4, scr);
		if (!user_input.empty()) {
			search_str = user_input;
			scr.reset_to_first_page();
		}
	}
	else {
		search_str = "";
		scr.reset_to_first_page();
	}
}

bool Keybinds::screen_change(const Screen &scr, WINDOW* left_pane, bool draw_right_pane) const {
	const unsigned int win_resize_width = draw_right_pane ? scr.get_x() : scr.get_x() / 2;
	draw_right_pane = !draw_right_pane;
	wresize(left_pane, scr.get_y(), win_resize_width);
	return draw_right_pane;
}

void Keybinds::help(const Screen &scr) const {
	const unsigned int help_win_y = 27;
	const unsigned int help_win_x = 80;
	WINDOW *help_win = newwin(help_win_y, help_win_x, scr.get_y() / 2 - help_win_y / 2,
			scr.get_x() / 2 - help_win_x / 2);
	mvwaddstr(help_win, 1, help_win_x / 2 - 5, "Keybinds:");
	mvwaddstr(help_win, 3, 1, "\"q\" - Quit or exit");
	mvwaddstr(help_win, 4, 1, "\"w\" or \"k\" or UP ARROW - Move cursor up");
	mvwaddstr(help_win, 5, 1, "\"s\" or \"j\" or DOWN ARROW - Move cursor down");
	mvwaddstr(help_win, 6, 1, "\"a\" or \"h\" or LEFT ARROW - Move back a directory");
	mvwaddstr(help_win, 7, 1, "\"d\" or \"l\" or RIGHT ARROW - Move into a directory");
	mvwaddstr(help_win, 8, 1, "\";\" - Split screen toggle");
	mvwaddstr(help_win, 9, 1, "SPACEBAR - Jump to specific line number (leave empty to cancel)");
	mvwaddstr(help_win, 10, 1, "\"=\" or PAGE UP - Go forward a page");
	mvwaddstr(help_win, 11, 1, "\"-\" or PAGE DOWN - Go back a page");
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
