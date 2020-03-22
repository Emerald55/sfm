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
		timeout(0); //draw window first time instantly
		while (is_running) {
			file_io fio(std::filesystem::current_path().string(), argc, argv, kb.search_str);
			ui.check_resize();
			if (fio.current_dir_size != 0) {
				const int page_floor = ui.page - ui.term_height;
				fio.current_dir_files.erase(fio.current_dir_files.begin(),
					       	fio.current_dir_files.begin() + page_floor); //trim before page
				if (fio.current_dir_size > ui.page) {
					fio.current_dir_files.erase(fio.current_dir_files.begin() + ui.page - page_floor,
						       	fio.current_dir_files.end()); //trim after
				}
				fio.selected_filepath = fio.current_dir_files[ui.curs_y];
				try {
					if (std::filesystem::is_directory(fio.selected_filepath) && ui.draw_selected_path) {
						fio.selected_dir_files = fio.get_dir_files(fio.selected_filepath, argc, argv);
						std::sort(fio.selected_dir_files.begin(), fio.selected_dir_files.end());
					}
				}
				catch (const std::filesystem::filesystem_error &) {} //no permission to read contents
			}
			werase(ui.current_dir_win);
			werase(ui.selected_dir_win);
			box(ui.current_dir_win, 0, 0);
			std::vector<std::string> file_content;
			ui.draw_window_files(fio.current_dir_files, ui.current_dir_win, argc, argv, true);
			ui.draw_window_title(fio.current_path, ui.current_dir_win);
			fio.contents_printable = fio.file_contents_printable(fio.selected_filepath);
			if (fio.contents_printable && ui.draw_selected_path) {
				file_content = fio.file_contents(fio.selected_filepath, ui.term_height);
			}
			if (ui.draw_selected_path) {
				box(ui.selected_dir_win, 0, 0);
				if (std::filesystem::is_directory(fio.selected_filepath)) {
					ui.draw_window_files(fio.selected_dir_files, ui.selected_dir_win, argc, argv);
				}
				else {
					ui.draw_window_file_contents(ui.selected_dir_win, file_content, fio.contents_printable);
				}
				ui.draw_window_title(fio.selected_filepath, ui.selected_dir_win);
				ui.draw_info(ui.selected_dir_win, fio.current_dir_size, fio.selected_filepath);
				wrefresh(ui.selected_dir_win);
			}
			else {
				ui.draw_info(ui.current_dir_win, fio.current_dir_size, fio.selected_filepath);
			}
			wrefresh(ui.current_dir_win);
			wchar_t input = getch();
			timeout(ui.update_speed);
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
					kb.move_right(fio.selected_filepath);
					break;
				case 'w':
				case 'k':
				case KEY_UP:
					if (fio.current_dir_files.size() > 0) {
						kb.move_up();
					}
					break;
				case 's':
				case 'j':
				case KEY_DOWN:
					kb.move_down(fio.current_dir_files.size(), fio.current_dir_size);
					break;
				case 'm':
					if (fio.current_dir_files.size() > 0) {
						ui.curs_y = 0;
					}
					break;
				case 'n':
					kb.jump_to_bottom(fio.current_dir_files.size());
					break;
				case '-':
					kb.up_page();
					break;
				case '=':
					kb.down_page(fio.current_dir_files.size(), fio.current_dir_size);
					break;
				case ' ':
					kb.jump_to_line(fio.current_dir_size);
					break;
				case 'v':
					if (fio.current_dir_files.size() > 0) {
						kb.edit_text(fio.selected_filepath);
					}
					break;
				case 'y':
					if (fio.current_dir_files.size() > 0) {
						kb.pager(fio.selected_filepath);
					}
					break;
				case 'u':
					kb.spawn_shell();
					break;
				case 'i':
				case '\n':
					if (fio.current_dir_files.size() > 0) {
						kb.xdg_open(fio.selected_filepath);
					}
					break;
				case 'g':
					if (fio.current_dir_files.size() > 0) {
						kb.remove(fio.selected_filepath);
					}
					break;
				case 'e':
					if (fio.current_dir_files.size() > 0) {
						kb.rename(fio.selected_filepath);
					}
					break;
				case 'c':
					if (fio.current_dir_files.size() > 0) {
						kb.copy(fio.selected_filepath);
					}
					break;
				case 'x':
					if (fio.current_dir_files.size() > 0) {
						kb.cut(fio.selected_filepath);
					}
					break;
				case 'p':
					kb.paste(fio.current_path);
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
