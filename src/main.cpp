#include <ncurses.h>
#include <iostream>
#include <vector>
#include <filesystem>
#include <unistd.h>
#include <algorithm>
#include <locale.h>
#include "keybinds.h"
#include "user_interface.h"
#include "file_io.h"

bool check_for_flag(int argc, char* argv[], const std::string &flag) {
	for (int i = 0; i < argc; i++) {
		if (flag == std::string(argv[i])) {
			return true;
		}
	}
	return false;
}

void user_interface::draw_window(const std::string &path, std::vector<std::string> files, WINDOW *win, 
		std::vector<std::string> file_contents, const int &argc, char* argv[],
	       	const bool &draw_curs = false) {
	if (file_contents.empty()) { //draw filenames
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
			if (num_format.size() + 1 + file.size() > current_scr_size) {
				file = file.substr(0, current_scr_size - num_format.size() - 1);
			}
			mvwaddstr(win, i + 1, 2 + num_format.size(), file.c_str());
			wattroff(win, COLOR_PAIR(1));
			wattroff(win, COLOR_PAIR(2));
		}
	}
	else { //draw file contents
		for (size_t i = 0; i < file_contents.size(); i++) {
			if (file_contents[i].size() > scr_x / 2 - 2) {
				file_contents[i] = file_contents[i].substr(0, scr_x / 2 - 2); //remove off-screen lines
			}
			if (i > term_height - 1) { break; }
			mvwaddstr(win, i + 1, 1, file_contents[i].c_str());
		}
	}
	box(win, 0, 0);
	draw_window_title(path, win);
}

int main(int argc, char *argv[]) {
	for (int i = 0; i < argc; i++) {
		if (std::string(argv[i]) == "-p") {
			errno = chdir(argv[i + 1]);
			if (errno == -1) {
				std::cout << "Could Not Find And/Or Enter Directory, Exiting...\n";
				return 0;
			}
			break;
		}
	}
	if (check_for_flag(argc, argv, "--help") || check_for_flag(argc, argv, "-help")) {
		std::cout << "Sfm Usage:\n";
		std::cout << "	--help -help      Displays help options\n";
		std::cout << "	-a                Unhides files starting with a \".\"\n";
		std::cout << "	-s                Shows symlinks\n";
		std::cout << "	-p /start/path    Path to start program from\n";
	}
	else {
		setlocale(LC_ALL, "");
		initscr();
		start_color();
		init_pair(1, COLOR_WHITE, COLOR_RED); //cursor
		init_pair(2, COLOR_YELLOW, COLOR_BLACK); //directories
		init_pair(3, COLOR_CYAN, COLOR_BLACK); //line numbers
		init_pair(4, COLOR_GREEN, COLOR_BLACK); //green input box
		init_pair(5, COLOR_RED, COLOR_BLACK); //red input box
		init_pair(6, COLOR_BLACK, COLOR_GREEN); //help menu
		cbreak();
		noecho();
		curs_set(0);
		keypad(stdscr, true);
		const unsigned int update_speed = 100;
		timeout(update_speed);
		user_interface ui;
		WINDOW *current_dir_win = newwin(ui.scr_y, ui.scr_x / 2, 0, 0);
		WINDOW *selected_dir_win = newwin(ui.scr_y, ui.scr_x / 2, 0, ui.scr_x / 2);
		keybinds kb;
		for (;;) {
			unsigned int check_scr_y, check_scr_x;
			getmaxyx(stdscr, check_scr_y, check_scr_x);
			if (check_scr_y != ui.scr_y || check_scr_x != ui.scr_x) { //check for screen resize
				ui.scr_y = check_scr_y;
				ui.scr_x = check_scr_x;
				wresize(current_dir_win, ui.scr_y, ui.scr_x / 2);
				if (!ui.draw_selected_path) {
					wresize(current_dir_win, ui.scr_y, ui.scr_x);
				}
				wresize(selected_dir_win, ui.scr_y, ui.scr_x / 2);
				mvwin(selected_dir_win, 0, ui.scr_x / 2);
				ui.term_height = ui.scr_y - 2;
				ui.page = 1 * ui.term_height;
			}
			std::string current_path = std::filesystem::current_path().string();
			if (current_path != "/") { current_path += "/"; }
			std::vector<std::string> current_dir_files = file_io::get_dir_files(current_path, argc, argv, kb.search_str);
			std::sort(current_dir_files.begin(), current_dir_files.end());
			size_t current_dir_size = current_dir_files.size();
			std::string selected_filepath = "?";
			std::vector<std::string> selected_dir_files;
			std::string perm_bits_and_owner;
			if (current_dir_size != 0) {
				const int page_floor = ui.page - ui.term_height;
				//trim before page
				current_dir_files.erase(current_dir_files.begin(), current_dir_files.begin() + page_floor);
				if (current_dir_size > ui.page) {
					current_dir_files.erase(current_dir_files.begin() + ui.page - page_floor,
						       	current_dir_files.end()); //trim after
				}
				selected_filepath = current_dir_files[ui.curs_y];
				perm_bits_and_owner = file_io::get_permbits(selected_filepath);
				try {
					if (std::filesystem::is_directory(selected_filepath) && ui.draw_selected_path) {
						selected_dir_files = file_io::get_dir_files(selected_filepath, argc, argv);
						std::sort(selected_dir_files.begin(), selected_dir_files.end());
					}
				}
				catch (const std::filesystem::filesystem_error &) {} //no permission to read contents
			}
			werase(current_dir_win);
			werase(selected_dir_win);
			std::vector<std::string> file_content;
			ui.draw_window(current_path, current_dir_files, current_dir_win, file_content, argc, argv, true);
			if (!std::filesystem::is_directory(selected_filepath)) {
				file_content = file_io::file_contents(selected_filepath, ui.term_height);
			}
			ui.draw_info(current_dir_win, ui.page, current_dir_size);
			if (ui.draw_selected_path) {
				ui.draw_window(selected_filepath, selected_dir_files, selected_dir_win, file_content, argc, argv);
				mvwaddstr(selected_dir_win, ui.scr_y - 1, ui.scr_x / 2 - 2 - perm_bits_and_owner.size(),
					       	perm_bits_and_owner.c_str());
				wrefresh(selected_dir_win);
			}
			else {
				mvwaddstr(current_dir_win, ui.scr_y - 1, (ui.scr_x / 2) - (perm_bits_and_owner.size() / 2),
					       	perm_bits_and_owner.c_str());
			}
			wrefresh(current_dir_win);
			wchar_t input = getch();
			if (input == 'q') { //quit
				std::string user_input = ui.input(" Quit? [Y/n]: ", 16, 5);
				if (user_input.empty() || user_input == "y" || user_input == "Y") {
					break;
				}
			}
			switch (input) {
				case 'a':
				case 'h':
				case KEY_LEFT:
					kb.move_left(ui);
					break;
				case 'd':
				case 'l':
				case KEY_RIGHT:
					kb.move_right(ui, selected_filepath);
					break;
				case 'w':
				case 'k':
				case KEY_UP:
					kb.move_up(ui, current_dir_files.size());
					break;
				case 's':
				case 'j':
				case KEY_DOWN:
					kb.move_down(ui, current_dir_files.size(), current_dir_size);
					break;
				case 'm':
					kb.jump_to_top(ui);
					break;
				case 'n':
					kb.jump_to_bottom(ui, current_dir_files.size());
					break;
				case '-':
					kb.up_page(ui);
					break;
				case '=':
					kb.down_page(ui, current_dir_files.size(), current_dir_size);
					break;
				case ' ':
					kb.jump_to_line(ui, current_dir_size);
					break;
				case 'v':
					kb.edit_text(selected_filepath);
					break;
				case 'u':
					kb.spawn_shell();
					break;
				case 'i':
				case '\n':
					kb.xdg_open(selected_filepath, current_dir_size);
					break;
				case 'g':
					kb.remove(ui, selected_filepath);
					break;
				case 'e':
					kb.rename(ui, selected_filepath, current_dir_files.size());
					break;
				case 'c':
					kb.copy(ui, selected_filepath, current_dir_files.size());
					break;
				case 'x':
					kb.cut(ui, selected_filepath);
					break;
				case 'p':
					kb.paste(ui, current_path);
					break;
				case 'b':
					kb.search(ui);
					break;
				case ';':
					kb.screen_change(ui, current_dir_win);
					break;
				case '?':
					kb.help(ui, update_speed);
					break;
			}
		}
		endwin();
	}
	return 0;
}
