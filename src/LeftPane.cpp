#include <algorithm>
#include <string>
#include <filesystem>
#include <cmath>
#include "LeftPane.h"
#include "RightPane.h"
#include "FileIO.h"
#include "Screen.h"
#include "FlagParse.h"

LeftPane::LeftPane(unsigned int width, unsigned int height) {
	this->width = width;
	this->height = height;
	pane = newwin(height, width, 0, 0);
}

void LeftPane::update(Screen &scr, bool draw_right_pane,
	       	const FlagParse &flags) {
	werase(pane);
	current_path = std::filesystem::current_path().string();
	files.clear();
	size = 0;
	selected_filepath = "?";
	box(pane, 0, 0);
	files = FileIO::get_dir_files(current_path, flags, kb.get_search_str());
	std::sort(files.begin(), files.end());
	size = files.size();
	if (size != 0) {
		const unsigned int current_line_number = (scr.get_page() - scr.get_term_height())
		       	+ scr.get_curs_y() + 1;
		if (size < current_line_number) { //if the last file on a page is deleted move to new last page
			scr.set(std::nullopt, static_cast<unsigned int>(std::ceil(static_cast<double>(size)
						/ static_cast<double>(scr.get_term_height()))) * scr.get_term_height());
		}
		const unsigned int page_floor = scr.get_page() - scr.get_term_height();
		files.erase(files.begin(), files.begin() + page_floor); //trim before current page
		if (size > scr.get_page()) {
			files.erase(files.begin() + scr.get_page() - page_floor, files.end()); //trim after current page
		}
		if (size < current_line_number) { //if hovering over last file and its deleted move cursor to new last file
			scr.set(files.size() - 1, std::nullopt);
		}
		selected_filepath = files[scr.get_curs_y()];
	}
	draw_window_files(scr, flags, true);
	draw_window_title(current_path);
	if (!draw_right_pane) {
		draw_window_info(scr, size, selected_filepath, searching);
	}
	wrefresh(pane);
}

bool LeftPane::get_input(Screen &scr, RightPane &rp) {
	const wchar_t input = getch();
	switch (input) {
		case 'q':
			if (!kb.quit(scr)) {
				return false;
			}
			break;
		case 'a':
		case 'h':
		case KEY_LEFT:
			kb.move_left(scr);
			break;
		case 'd':
		case 'l':
		case KEY_RIGHT:
			kb.move_right(scr, selected_filepath);
			break;
		case 's':
		case 'j':
		case KEY_DOWN:
			kb.move_down(scr, files.size(), size);
			break;
		case 'n':
			kb.jump_to_bottom(scr, files.size());
			break;
		case '-':
		case KEY_PPAGE:
			kb.up_page(scr);
			break;
		case '=':
		case KEY_NPAGE:
			kb.down_page(scr, size);
			break;
		case ' ':
			kb.jump_to_line(scr, size);
			break;
		case 'u':
			kb.spawn_shell();
			break;
		case 'p':
			kb.paste(scr, current_path);
			break;
		case 'b':
			searching = kb.search(scr);
			break;
		case ';':
			rp.set_draw(kb.screen_change(scr, this, rp.get_draw()));
			break;
		case '?':
			kb.help(scr);
	}
	if (files.size() > 0) {
		switch (input) {
			case 'w':
			case 'k':
			case KEY_UP:
				kb.move_up(scr);
				break;
			case 'm':
				scr.set(0, std::nullopt);
				break;
			case 'x':
				kb.cut(scr, selected_filepath);
				break;
			case 'c':
				kb.copy(scr, selected_filepath);
				break;
			case 'i':
			case '\n':
				kb.xdg_open(selected_filepath);
				break;
			case 'g':
				kb.remove(scr, selected_filepath);
				break;
			case 'e':
				kb.rename(scr, selected_filepath);
				break;
			case 'v':
				kb.edit_text(selected_filepath);
				break;
			case 'y':
				kb.pager(selected_filepath);
		}
	}
	timeout(scr.update_speed);
	return true;
}
