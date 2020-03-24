#include <ncurses.h>
#include <iostream>
#include <unistd.h>
#include <filesystem>
#include <vector>
#include <sys/wait.h>
#include "user_interface.h"
#include "file_io.h"
#include "keybinds.h"

keybinds::keybinds(user_interface *ui) {
	this->ui = ui;
}

void keybinds::get_input(const std::string &selected_filepath, unsigned int left_pane_size_currently, 
		unsigned int left_pane_size, const std::string &current_path, bool &is_running) {
	wchar_t input = getch();
	switch (input) {
		case 'q':
			is_running = quit();
			break;
		case 'a':
		case 'h':
		case KEY_LEFT:
			move_left();
			break;
		case 'd':
		case 'l':
		case KEY_RIGHT:
			move_right(selected_filepath);
			break;
		case 'w':
		case 'k':
		case KEY_UP:
			if (left_pane_size_currently > 0) {
				move_up();
			}
			break;
		case 's':
		case 'j':
		case KEY_DOWN:
			move_down(left_pane_size_currently, left_pane_size);
			break;
		case 'm':
			if (left_pane_size_currently > 0) {
				ui->curs_y = 0;
			}
			break;
		case 'n':
			jump_to_bottom(left_pane_size_currently);
			break;
		case '-':
			up_page();
			break;
		case '=':
			down_page(left_pane_size_currently, left_pane_size);
			break;
		case ' ':
			jump_to_line(left_pane_size);
			break;
		case 'v':
			if (left_pane_size_currently > 0) {
				edit_text(selected_filepath);
			}
			break;
		case 'y':
			if (left_pane_size_currently > 0) {
				pager(selected_filepath);
			}
			break;
		case 'u':
			spawn_shell();
			break;
		case 'i':
		case '\n':
			if (left_pane_size_currently > 0) {
				xdg_open(selected_filepath);
			}
			break;
		case 'g':
			if (left_pane_size_currently > 0) {
				remove(selected_filepath);
			}
			break;
		case 'e':
			if (left_pane_size_currently > 0) {
				rename(selected_filepath);
			}
			break;
		case 'c':
			if (left_pane_size_currently > 0) {
				copy(selected_filepath);
			}
			break;
		case 'x':
			if (left_pane_size_currently > 0) {
				cut(selected_filepath);
			}
			break;
		case 'p':
			paste(current_path);
			break;
		case 'b':
			search();
			break;
		case ';':
			screen_change();
			break;
		case '?':
			help();
	}
}

bool keybinds::quit() {
	std::string user_input = ui->input(" Quit? [Y/n]: ", 5, 2);
	if (user_input.empty() || user_input == "y" || user_input == "Y") {
		return false;
	}
	return true;
}

void keybinds::move_left() {
	ui->curs_y = 0;
	ui->page = ui->term_height;
	search_str = "";
	chdir("..");
}

void keybinds::move_right(const std::string &selected_filepath) {
	if (std::filesystem::is_directory(selected_filepath)) {
		int err = chdir(selected_filepath.c_str());
		if (err == -1) {
			ui->alert_box(" Invalid Permission ", 20, 750, 5);
		}
		else {
			ui->page = ui->term_height;
			ui->curs_y = 0;
			search_str = "";
		}
	}
}

void keybinds::move_up() {
	if (ui->curs_y > 0) {
		ui->curs_y -= 1;
	}
	else if (ui->page != ui->term_height) {
		ui->curs_y = ui->term_height - 1;
		ui->page -= ui->term_height;
	}
}

void keybinds::move_down(unsigned int left_pane_size_currently, 
		size_t left_pane_size) {
	if (left_pane_size_currently > 0) {
		if (static_cast<unsigned>(ui->curs_y) < left_pane_size_currently - 1) {
			ui->curs_y += 1;
		}
		else if ((ui->curs_y + 1 + ui->page - ui->scr_y + 2) != left_pane_size) {
			ui->page += ui->term_height;
			ui->curs_y = 0;
		}
	}
}

void keybinds::jump_to_bottom(unsigned int left_pane_size_currently) {
	if (left_pane_size_currently > 0) {
		ui->curs_y = left_pane_size_currently - 1;
	}
}

void keybinds::up_page() {
	if (ui->page != ui->term_height) {
		ui->page -= ui->term_height;
		ui->curs_y = 0;
	}
}

void keybinds::down_page(unsigned int left_pane_size_currently, 
		size_t left_pane_size) {
	if (left_pane_size_currently % ui->term_height == 0 &&
			left_pane_size > ui->term_height + 1) {
		ui->page += ui->term_height;
		ui->curs_y = 0;
	}
}

void keybinds::jump_to_line(size_t left_pane_size) {
	std::string user_input = ui->input(" Jump To: ", 4, 10);
	try {
		if (!user_input.empty() && std::stoul(user_input) > 0 && std::stoul(user_input) <
				left_pane_size + 1) {
			ui->curs_y = std::stoi(user_input) % ui->term_height - 1;
			if (std::stoul(user_input) == ui->term_height || ui->curs_y == -1) {
				ui->curs_y = ui->term_height - 1;
			}
			ui->page = round_to(std::stoi(user_input), ui->term_height);
		}
	}
	catch (const std::invalid_argument &) {
		ui->alert_box(" Not A Number ", 14, 750, 5);
	}
	catch (const std::out_of_range &) {
		ui->alert_box(" Too large of a number ", 23, 750, 5);
	}
}

void keybinds::edit_text(const std::string &selected_filepath) {
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

void keybinds::pager(const std::string &selected_filepath) {
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

void keybinds::spawn_shell() {
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

void keybinds::xdg_open(const std::string &selected_filepath) {
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

void keybinds::remove(const std::string &selected_filepath) {
	std::string user_input = ui->input(" Delete File/Directory? [y/N]: ", 5, 2);
	try {
		if (std::filesystem::exists(selected_filepath) &&
				(user_input == "Y" || user_input == "y")) {
			std::filesystem::remove_all(selected_filepath);
			ui->curs_y = 0;
			ui->page = ui->term_height;
		}
	}
	catch (const std::filesystem::filesystem_error &) {
		ui->alert_box(" Delete Failed ", 15, 750, 5);
	}
}

void keybinds::rename(const std::string &selected_filepath) {
	std::string user_input = ui->input(" Rename: ", 4);
	if (!user_input.empty() && std::filesystem::exists(selected_filepath)) {
		try {
			std::filesystem::rename(selected_filepath, user_input);
		}
		catch (const std::filesystem::filesystem_error &) {
			ui->alert_box(" Invalid Filename ", 18, 750, 5);
		}
	}
}

void keybinds::copy(const std::string &selected_filepath) {
	copy_path = selected_filepath;
	cut_path = false;
	ui->alert_box((" Copied: " + file_io::path_to_filename(selected_filepath) + " ").c_str(),
			10 + file_io::path_to_filename(selected_filepath).size(), 500, 4);
}

void keybinds::cut(const std::string &selected_filepath) {
	std::string user_input = ui->input(" Cut File/Directory? [y/N]: ", 5, 2);
	if (std::filesystem::exists(selected_filepath) &&
			(user_input == "y" || user_input == "Y")) {
		copy_path = selected_filepath;
		cut_path = true;
	}
}

void keybinds::paste(const std::string &current_path) {
	if (std::filesystem::exists(copy_path)) {
		try {
			std::filesystem::copy(copy_path, current_path + "/" + file_io::path_to_filename(copy_path),
					std::filesystem::copy_options::recursive);
			if (cut_path) {
				ui->alert_box((" Cut: " + file_io::path_to_filename(copy_path) + " ").c_str(),
						7 + file_io::path_to_filename(copy_path).size(), 500, 4);
				std::filesystem::remove_all(copy_path);
				ui->curs_y = 0;
				ui->page = ui->term_height;
				cut_path = false;
			}
			else {
				ui->alert_box((" Pasted: " + file_io::path_to_filename(copy_path) + " ").c_str(),
						10 + file_io::path_to_filename(copy_path).size(), 500, 4);
			}
		}
		catch (std::exception& e) {
			if (cut_path) {
				ui->alert_box(" Cut Failed ", 12, 750, 5);
				cut_path = false;
			}
			else {
				ui->alert_box(" Paste Failed ", 14, 750, 5);
			}
		}
	}
}

void keybinds::search() {
	if (search_str.empty()) {
		std::string user_input = ui->input(" Search: ", 4);
		if (!user_input.empty()) {
			search_str = user_input;
			ui->curs_y = 0;
			ui->page = ui->term_height;
		}
	}
	else {
		search_str = "";
		ui->curs_y = 0;
		ui->page = ui->term_height;
	}
}

void keybinds::screen_change() {
	const unsigned int win_resize_width = ui->draw_right_pane ? ui->scr_x : ui->scr_x / 2;
	ui->draw_right_pane = !ui->draw_right_pane;
	wresize(ui->left_pane, ui->scr_y, win_resize_width);
}

void keybinds::help() {
	const unsigned int help_win_y = 27;
	const unsigned int help_win_x = 80;
	WINDOW *help_win = newwin(help_win_y, help_win_x, ui->scr_y / 2 - help_win_y / 2,
			ui->scr_x / 2 - help_win_x / 2);
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
	timeout(ui->update_speed);
}
