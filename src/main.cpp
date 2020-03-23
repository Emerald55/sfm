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

bool parse_flags(bool &show_symbolic_links, bool &show_hidden_files, int argc, char* argv[]) {
	int opt;
	while((opt = getopt(argc, argv, ":p:has")) != -1) {
		switch (opt) {
			case '?':
			       std::cerr << "Invalid flag, type \"-h\" for options.\n";
			       return false;
			case ':':
			       std::cerr << "A flag is missing an extra value.\n";
			       return false;
			case 'a':
			       show_hidden_files = true;
			       break;
			case 's':
			       show_symbolic_links = true;
			       break;
			case 'h':
				std::cout << "Sfm Usage:\n";
				std::cout << "	-h                Displays help options\n";
				std::cout << "	-a                Unhides files starting with a \".\"\n";
				std::cout << "	-s                Shows symlinks\n";
				std::cout << "	-p /start/path    Path to start program from\n";
				return false;
			case 'p':
				errno = chdir(optarg);
				if (errno == -1) {
					std::cerr << "Could Not Find And/Or Enter Directory, Exiting...\n";
					return false;
				}
		}
	}
	return true;
}

int main(int argc, char *argv[]) {
	bool show_symbolic_links = false;
	bool show_hidden_files = false;
	const bool start_program = parse_flags(show_symbolic_links, show_hidden_files, argc, argv);
	setlocale(LC_ALL, "");
	initscr();
	user_interface ui;
	keybinds kb(&ui);
	bool is_running = true;
	timeout(0); //draw window first time instantly
	while (is_running && start_program) {
		file_io fio(std::filesystem::current_path().string(), show_hidden_files, kb.search_str);
		ui.check_resize();
		if (fio.left_pane_size != 0) {
			const int page_floor = ui.page - ui.term_height;
			fio.left_pane_files.erase(fio.left_pane_files.begin(),
					fio.left_pane_files.begin() + page_floor); //trim before page
			if (fio.left_pane_size > ui.page) {
				fio.left_pane_files.erase(fio.left_pane_files.begin() + ui.page - page_floor,
						fio.left_pane_files.end()); //trim after
			}
			fio.selected_filepath = fio.left_pane_files[ui.curs_y];
			try {
				if (std::filesystem::is_directory(fio.selected_filepath) && ui.draw_selected_path) {
					fio.right_pane_files = fio.get_dir_files(fio.selected_filepath, show_hidden_files);
					std::sort(fio.right_pane_files.begin(), fio.right_pane_files.end());
				}
			}
			catch (const std::filesystem::filesystem_error &) {} //no permission to read contents
		}
		werase(ui.left_pane);
		werase(ui.right_pane);
		box(ui.left_pane, 0, 0);
		std::vector<std::string> file_content;
		ui.draw_window_files(fio.left_pane_files, ui.left_pane, show_symbolic_links, true);
		ui.draw_window_title(fio.current_path, ui.left_pane);
		fio.contents_printable = fio.file_contents_printable(fio.selected_filepath);
		if (fio.contents_printable && ui.draw_selected_path) {
			file_content = fio.get_file_contents(fio.selected_filepath, ui.term_height);
		}
		if (ui.draw_selected_path) {
			box(ui.right_pane, 0, 0);
			if (std::filesystem::is_directory(fio.selected_filepath)) {
				ui.draw_window_files(fio.right_pane_files, ui.right_pane, show_symbolic_links);
			}
			else {
				ui.draw_window_file_contents(ui.right_pane, file_content, fio.contents_printable);
			}
			ui.draw_window_title(fio.selected_filepath, ui.right_pane);
			ui.draw_info(ui.right_pane, fio.left_pane_size, fio.selected_filepath);
			wrefresh(ui.right_pane);
		}
		else {
			ui.draw_info(ui.left_pane, fio.left_pane_size, fio.selected_filepath);
		}
		wrefresh(ui.left_pane);
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
				if (fio.left_pane_files.size() > 0) {
					kb.move_up();
				}
				break;
			case 's':
			case 'j':
			case KEY_DOWN:
				kb.move_down(fio.left_pane_files.size(), fio.left_pane_size);
				break;
			case 'm':
				if (fio.left_pane_files.size() > 0) {
					ui.curs_y = 0;
				}
				break;
			case 'n':
				kb.jump_to_bottom(fio.left_pane_files.size());
				break;
			case '-':
				kb.up_page();
				break;
			case '=':
				kb.down_page(fio.left_pane_files.size(), fio.left_pane_size);
				break;
			case ' ':
				kb.jump_to_line(fio.left_pane_size);
				break;
			case 'v':
				if (fio.left_pane_files.size() > 0) {
					kb.edit_text(fio.selected_filepath);
				}
				break;
			case 'y':
				if (fio.left_pane_files.size() > 0) {
					kb.pager(fio.selected_filepath);
				}
				break;
			case 'u':
				kb.spawn_shell();
				break;
			case 'i':
			case '\n':
				if (fio.left_pane_files.size() > 0) {
					kb.xdg_open(fio.selected_filepath);
				}
				break;
			case 'g':
				if (fio.left_pane_files.size() > 0) {
					kb.remove(fio.selected_filepath);
				}
				break;
			case 'e':
				if (fio.left_pane_files.size() > 0) {
					kb.rename(fio.selected_filepath);
				}
				break;
			case 'c':
				if (fio.left_pane_files.size() > 0) {
					kb.copy(fio.selected_filepath);
				}
				break;
			case 'x':
				if (fio.left_pane_files.size() > 0) {
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
		}
	}
	endwin();
	return 0;
}
