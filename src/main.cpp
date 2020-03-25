#include <ncurses.h>
#include <vector>
#include <filesystem>
#include <locale.h>
#include "keybinds.h"
#include "user_interface.h"
#include "flag_parse.h"
#include "file_io.h"
#include "left_pane.h"
#include "right_pane.h"

int main(int argc, char *argv[]) {
	bool show_symbolic_links = false;
	bool show_hidden_files = false;
	const bool start_program = parse_flags(show_symbolic_links, show_hidden_files,
		       	argc, argv);
	setlocale(LC_ALL, "");
	initscr();
	user_interface ui;
	keybinds kb(&ui);
	bool is_running = true;
	timeout(0); //draw window first time instantly
	while (is_running && start_program) {
		const std::string current_path = std::filesystem::current_path().string();
		ui.check_resize();
		left_pane lp(ui.curs_y, show_hidden_files, kb.search_str, current_path, ui.page,
			       	ui.term_height);
		werase(ui.left_pane);
		werase(ui.right_pane);
		box(ui.left_pane, 0, 0);
		ui.draw_window_files(lp.files, ui.left_pane, show_symbolic_links, true);
		ui.draw_window_title(current_path, ui.left_pane);
		if (ui.draw_right_pane) {
			right_pane rp(lp.selected_filepath, show_hidden_files, ui.draw_right_pane,
				       	lp.size);
			if (file_io::file_contents_printable(lp.selected_filepath)) {
				rp.file_content = file_io::get_file_contents(lp.selected_filepath,
					       	ui.term_height);
			}
			box(ui.right_pane, 0, 0);
			if (std::filesystem::is_directory(lp.selected_filepath)) {
				ui.draw_window_files(rp.files, ui.right_pane, show_symbolic_links);
			}
			else {
				bool contents_printable = true;
				for (std::string &line : rp.file_content) {
					if (line.find('\0') != std::string::npos) {
						contents_printable = false;
						break;
					}
				}
				ui.draw_window_file_contents(ui.right_pane, rp.file_content, 
						contents_printable);
			}
			ui.draw_window_title(lp.selected_filepath, ui.right_pane);
			ui.draw_info(ui.right_pane, lp.size, lp.selected_filepath);
			wrefresh(ui.right_pane);
		}
		else {
			ui.draw_info(ui.left_pane, lp.size, lp.selected_filepath);
		}
		wrefresh(ui.left_pane);
		kb.get_input(lp.selected_filepath, lp.files.size(), lp.size, current_path,
			       	is_running);
		timeout(ui.update_speed);
	}
	endwin();
	return 0;
}
