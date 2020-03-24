#include <ncurses.h>
#include <iostream>
#include <vector>
#include <filesystem>
#include <unistd.h>
#include <algorithm>
#include <locale.h>
#include "keybinds.h"
#include "user_interface.h"
#include "flag_parse.h"
#include "file_io.h"

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
		ui.check_resize();
		file_io fio(std::filesystem::current_path().string(), show_hidden_files, kb.search_str);
		fio.set_dir_files(ui.page, ui.term_height, ui.curs_y, ui.right_pane, show_hidden_files);
		werase(ui.left_pane);
		werase(ui.right_pane);
		box(ui.left_pane, 0, 0);
		ui.draw_window_files(fio.left_pane_files, ui.left_pane, show_symbolic_links, true);
		ui.draw_window_title(fio.current_path, ui.left_pane);
		fio.contents_printable = fio.file_contents_printable(fio.selected_filepath);
		std::vector<std::string> file_content;
		if (fio.contents_printable && ui.draw_right_pane) {
			file_content = fio.get_file_contents(fio.selected_filepath, ui.term_height);
		}
		if (ui.draw_right_pane) {
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
		kb.get_input(fio.selected_filepath, fio.left_pane_files.size(), fio.left_pane_size, 
				fio.current_path, is_running);
		timeout(ui.update_speed);
	}
	endwin();
	return 0;
}
