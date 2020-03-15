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

inline bool file_contents_printable(const std::string &path) {
	if (std::filesystem::is_directory(path) ||
		       	std::filesystem::is_block_file(path) ||
		       	std::filesystem::is_character_file(path) ||
		       	std::filesystem::is_fifo(path) ||
		       	std::filesystem::is_socket(path)) {
		return false;
	}
	return true;
}

bool check_for_flag(int argc, char* argv[], const std::string &flag) {
	for (int i = 0; i < argc; i++) {
		if (flag == std::string(argv[i])) {
			return true;
		}
	}
	return false;
}

int main(int argc, char *argv[]) {
	for (int i = 0; i < argc; i++) {
		if (std::string(argv[i]) == "-p") {
			errno = chdir(argv[i + 1]);
			if (errno == -1) {
				std::cerr << "Could Not Find And/Or Enter Directory, Exiting...\n";
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
		user_interface ui;
		keybinds kb(&ui);
		bool is_running = true;
		while (is_running) {
			ui.check_resize();
			std::string current_path = std::filesystem::current_path().string();
			std::vector<std::string> current_dir_files = file_io::get_dir_files(current_path, argc,
				       	argv, kb.search_str);
			std::sort(current_dir_files.begin(), current_dir_files.end());
			size_t current_dir_size = current_dir_files.size();
			std::string selected_filepath = "?";
			std::vector<std::string> selected_dir_files;
			std::string perm_bits_and_owner;
			if (current_dir_size != 0) {
				const int page_floor = ui.page - ui.term_height;
				//trim before page
				current_dir_files.erase(current_dir_files.begin(),
					       	current_dir_files.begin() + page_floor);
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
			werase(ui.current_dir_win);
			werase(ui.selected_dir_win);
			std::vector<std::string> file_content;
			ui.draw_window_files(current_path, current_dir_files, ui.current_dir_win, argc, argv, true);
			if (file_contents_printable(selected_filepath) && ui.draw_selected_path) {
				file_content = file_io::file_contents(selected_filepath, ui.term_height);
			}
			ui.draw_info(ui.current_dir_win, ui.page, current_dir_size);
			if (ui.draw_selected_path) {
				if (file_content.empty()) {
					ui.draw_window_files(selected_filepath, selected_dir_files, ui.selected_dir_win,
						       	argc, argv);
				}
				else {
					ui.draw_window_file_contents(selected_filepath, ui.selected_dir_win, file_content);
				}
				mvwaddstr(ui.selected_dir_win, ui.scr_y - 1, ui.scr_x / 2 - 2 - perm_bits_and_owner.size(),
					       	perm_bits_and_owner.c_str());
				wrefresh(ui.selected_dir_win);
			}
			else {
				mvwaddstr(ui.current_dir_win, ui.scr_y - 1, (ui.scr_x / 2) - (perm_bits_and_owner.size() / 2),
					       	perm_bits_and_owner.c_str());
			}
			wrefresh(ui.current_dir_win);
			wchar_t input = getch();
			switch (input) {
				case 'q':
					is_running = kb.quit();
					break;
				case 'a':
				case 'h':
				case KEY_LEFT:
					kb.move_left();
					break;
				case 'd':
				case 'l':
				case KEY_RIGHT:
					kb.move_right(selected_filepath);
					break;
				case 'w':
				case 'k':
				case KEY_UP:
					if (current_dir_files.size() > 0) {
						kb.move_up();
					}
					break;
				case 's':
				case 'j':
				case KEY_DOWN:
					kb.move_down(current_dir_files.size(), current_dir_size);
					break;
				case 'm':
					if (current_dir_files.size() > 0) {
						ui.curs_y = 0;
					}
					break;
				case 'n':
					kb.jump_to_bottom(current_dir_files.size());
					break;
				case '-':
					kb.up_page();
					break;
				case '=':
					kb.down_page(current_dir_files.size(), current_dir_size);
					break;
				case ' ':
					kb.jump_to_line(current_dir_size);
					break;
				case 'v':
					if (current_dir_files.size() > 0) {
						kb.edit_text(selected_filepath);
					}
					break;
				case 'u':
					kb.spawn_shell();
					break;
				case 'i':
				case '\n':
					if (current_dir_files.size() > 0) {
						kb.xdg_open(selected_filepath);
					}
					break;
				case 'g':
					if (current_dir_files.size() > 0) {
						kb.remove(selected_filepath);
					}
					break;
				case 'e':
					if (current_dir_files.size() > 0) {
						kb.rename(selected_filepath);
					}
					break;
				case 'c':
					if (current_dir_files.size() > 0) {
						kb.copy(selected_filepath);
					}
					break;
				case 'x':
					if (current_dir_files.size() > 0) {
						kb.cut(selected_filepath);
					}
					break;
				case 'p':
					kb.paste(current_path);
					break;
				case 'b':
					kb.search();
					break;
				case ';':
					kb.screen_change();
					break;
				case '?':
					kb.help();
					break;
			}
		}
		endwin();
	}
	return 0;
}
