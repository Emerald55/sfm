#include <ncurses.h>
#include <iostream>
#include <unistd.h>
#include <filesystem>
#include <vector>
#include <sys/wait.h>
#include "user_interface.h"
#include "file_io.h"
#include "keybinds.h"

void keybinds::move_left(user_interface &ui) {
	ui.curs_y = 0;
	ui.page = ui.term_height;
	search_str = "";
	chdir("..");
}
void keybinds::move_right(user_interface &ui, const std::string &selected_filepath) {
	if (std::filesystem::is_directory(selected_filepath)) {
		int err = chdir(selected_filepath.c_str());
		if (err == -1) {
			ui.alert_box(" Invalid Permission ", 20, 750, 5);
		}
		else {
			ui.page = ui.term_height;
			ui.curs_y = 0;
			search_str = "";
		}
	}
}
void keybinds::move_up(user_interface &ui, const unsigned int current_dir_size_currently) {
	if (current_dir_size_currently > 0) {
		if (ui.curs_y > 0) {
			ui.curs_y -= 1;
		}
		else if (ui.page != ui.term_height) {
			ui.curs_y = ui.term_height - 1;
			ui.page -= ui.term_height;
		}
	}
}
void keybinds::move_down(user_interface &ui, const unsigned int current_dir_size_currently, 
		const size_t &current_dir_size) {
	if (current_dir_size_currently > 0) {
		if (static_cast<unsigned>(ui.curs_y) < current_dir_size_currently - 1) {
			ui.curs_y += 1;
		}
		else if ((ui.curs_y + 1 + ui.page - ui.scr_y + 2) != current_dir_size) {
			ui.page += ui.term_height;
			ui.curs_y = 0;
		}
	}
}
void keybinds::jump_to_top(user_interface &ui) {
	ui.curs_y = 0;
}
void keybinds::jump_to_bottom(user_interface &ui,
		const unsigned int current_dir_size_currently) {
	ui.curs_y = current_dir_size_currently - 1;
}
void keybinds::up_page(user_interface &ui) {
	if (ui.page != ui.term_height) {
		ui.page -= ui.term_height;
		ui.curs_y = 0;
	}
}
void keybinds::down_page(user_interface &ui, const unsigned int current_dir_size_currently, 
		const size_t &current_dir_size) {
	if (current_dir_size_currently % ui.term_height == 0 &&
			current_dir_size > ui.term_height + 1) {
		ui.page += ui.term_height;
		ui.curs_y = 0;
	}
}
void keybinds::jump_to_line(user_interface &ui, const size_t &current_dir_size) {
	std::string user_input = ui.input(" Jump To: ", 20, 4);
	try {
		if (std::stoul(user_input) > 0 && std::stoul(user_input) <
				current_dir_size + 1) {
			ui.curs_y = std::stoi(user_input) % ui.term_height - 1;
			if (std::stoul(user_input) == ui.term_height || ui.curs_y == -1) {
				ui.curs_y = ui.term_height - 1;
			}
			ui.page = round_to(std::stoi(user_input), ui.term_height);
		}
	}
	catch (const std::invalid_argument &) {
		ui.alert_box(" Not A Number ", 14, 750, 5);
	}
	catch (const std::out_of_range &) {
		ui.alert_box(" Too large of a number ", 23, 750, 5);
	}
}
void keybinds::edit_text(const std::string &selected_filepath,
	       	const size_t &current_dir_size) {
	if (current_dir_size != 0) {
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
				std::cout << "\"$EDITOR\" environmental variable not set.\n";
			}
			exit(1);
		}
		waitpid(pid, NULL, 0);
		refresh();
	}
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
			std::cout << "\"$SHELL\" environmental variable not set.\n";
		}
		exit(1);
	}
	waitpid(pid, NULL, 0);
	refresh();
}
void keybinds::xdg_open(const std::string &selected_filepath, const size_t &current_dir_size) {
	if (current_dir_size != 0) {
		def_prog_mode();
		endwin();
		pid_t pid = fork();
		if (pid == 0) {
			execl("/usr/bin/xdg-open", "xdg-open",
					selected_filepath.c_str(), (char*)0);
			exit(1);
		}
		waitpid(pid, NULL, 0);
		refresh();
	}
}
void keybinds::remove(user_interface &ui, const std::string &selected_filepath) {
	std::string user_input = ui.input(" Delete File/Directory? [y/N]: ", 33, 5);
	try {
		if (std::filesystem::exists(selected_filepath) &&
				(user_input == "Y" || user_input == "y")) {
			std::filesystem::remove_all(selected_filepath);
			ui.curs_y = 0;
			ui.page = ui.term_height;
		}
	}
	catch (const std::filesystem::filesystem_error &) {
		ui.alert_box(" Delete Failed ", 15, 750, 5);
	}
}
void keybinds::rename(user_interface &ui, const std::string &selected_filepath, 
		const unsigned int current_dir_size_currently) {
	if (current_dir_size_currently > 0) {
		std::string user_input = ui.input(" Rename: ", 20, 4);
		if (!user_input.empty() && std::filesystem::exists(selected_filepath)) {
			try {
				std::filesystem::rename(selected_filepath, user_input);
			}
			catch (const std::filesystem::filesystem_error &) {
				ui.alert_box(" Invalid Filename ", 18, 750, 5);
			}
		}
	}
}
void keybinds::copy(user_interface &ui, const std::string &selected_filepath, 
		const unsigned int current_dir_size_currently) {
	if (current_dir_size_currently > 0) {
		copy_path = selected_filepath;
		cut_path = false;
		ui.alert_box((" Copied: " + file_io::path_to_filename(selected_filepath) + " ").c_str(),
				10 + file_io::path_to_filename(selected_filepath).size(), 500, 4);
	}
}
void keybinds::cut(user_interface &ui, const std::string &selected_filepath) {
	std::string user_input = ui.input(" Cut File/Directory? [y/N]: ", 30, 5);
	if (std::filesystem::exists(selected_filepath) &&
			(user_input == "y" || user_input == "Y")) {
		copy_path = selected_filepath;
		cut_path = true;
	}
}
void keybinds::paste(user_interface &ui, const std::string &current_path) {
	if (std::filesystem::exists(copy_path)) {
		try {
			std::filesystem::copy(copy_path, current_path + file_io::path_to_filename(copy_path),
					std::filesystem::copy_options::recursive);
			if (cut_path) {
				ui.alert_box((" Cut: " + file_io::path_to_filename(copy_path) + " ").c_str(),
						7 + file_io::path_to_filename(copy_path).size(), 500, 4);
				std::filesystem::remove_all(copy_path);
				ui.curs_y = 0;
				ui.page = ui.term_height;
				cut_path = false;
			}
			else {
				ui.alert_box((" Pasted: " + file_io::path_to_filename(copy_path) + " ").c_str(),
						10 + file_io::path_to_filename(copy_path).size(), 500, 4);
			}
		}
		catch (std::exception& e) {
			if (cut_path) {
				ui.alert_box(" Cut Failed ", 12, 750, 5);
				cut_path = false;
			}
			else {
				ui.alert_box(" Paste Failed ", 14, 750, 5);
			}
		}
	}
}
void keybinds::search(user_interface &ui) {
	if (search_str.empty()) {
		std::string user_input = ui.input(" Search: ", 20, 4);
		if (!user_input.empty()) {
			search_str = user_input;
			ui.curs_y = 0;
			ui.page = ui.term_height;
		}
	}
	else {
		search_str = "";
		ui.curs_y = 0;
		ui.page = ui.term_height;
	}
}
void keybinds::screen_change(user_interface &ui, WINDOW *current_dir_win) {
	const unsigned int win_resize_width = ui.draw_selected_path ? ui.scr_x : ui.scr_x / 2;
	ui.draw_selected_path = !ui.draw_selected_path;
	wresize(current_dir_win, ui.scr_y, win_resize_width);
}
void keybinds::help(user_interface &ui, const unsigned int &update_speed) {
	const int help_win_y = 26;
	const int help_win_x = 80;
	WINDOW *help_win = newwin(help_win_y, help_win_x, ui.scr_y / 2 - help_win_y / 2,
			ui.scr_x / 2 - help_win_x / 2);
	mvwaddstr(help_win, 1, help_win_x / 2 - 5, "Keybinds:");
	mvwaddstr(help_win, 3, 1, "\"q\" - Quit or exit");
	mvwaddstr(help_win, 4, 1, "\"w\" or \"k\" or UP ARROW - Move cursor up");
	mvwaddstr(help_win, 5, 1, "\"s\" or \"j\" or DOWN ARROW - Move cursor down");
	mvwaddstr(help_win, 6, 1, "\"a\" or \"h\" or LEFT ARROW - Move back a directory");
	mvwaddstr(help_win, 7, 1, "\"d\" or \"l\" or RIGHT ARROW - Move into a directory");
	mvwaddstr(help_win, 8, 1, "\";\" - Split screen toggle");
	mvwaddstr(help_win, 9, 1, "SPACEBAR - Jump to specific line number");
	mvwaddstr(help_win, 10, 1, "\"=\" - Go forward a page");
	mvwaddstr(help_win, 11, 1, "\"-\" - Go back a page");
	mvwaddstr(help_win, 12, 1, "\"m\" - Jump to top of current page");
	mvwaddstr(help_win, 13, 1, "\"n\" - Jump to bottom of current page");
	mvwaddstr(help_win, 14, 1, "\"i\" or ENTER - Open selected item with xdg-open");
	mvwaddstr(help_win, 15, 1, "\"u\" - Spawn interactive shell");
	mvwaddstr(help_win, 16, 1, "\"v\" - Edit file or folder with default editor");
	mvwaddstr(help_win, 17, 1, "\"b\" - Search for file or folder (press again to stop)");
	mvwaddstr(help_win, 18, 1, "\"e\" - Rename file or folder (leave empty to cancel)");
	mvwaddstr(help_win, 19, 1, "\"g\" - Delete file or folder (if folder contents are also deleted)");
	mvwaddstr(help_win, 20, 1, "\"c\" - Copy a file or folder");
	mvwaddstr(help_win, 21, 1, "\"p\" - Paste a file or folder");
	mvwaddstr(help_win, 22, 1, "\"x\" - Cut a file or folder (deletes after paste rather than before)");
	mvwaddstr(help_win, 24, 1, "Press any key to exit help menu...");
	box(help_win, 0, 0);
	wrefresh(help_win);
	timeout(-1);
	getch();
	delwin(help_win);
	timeout(update_speed);
}
