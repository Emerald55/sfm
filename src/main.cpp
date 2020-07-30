#include <ncurses.h>
#include <filesystem>
#include <string>
#include <locale.h>
#include "flag_parse.h"
#include "keybinds.h"
#include "screen_info.h"
#include "left_pane.h"
#include "right_pane.h"

int main(int argc, char *argv[]) {
	flag_parse flags(argc, argv);
	if (flags.get_start_program()) {
		setlocale(LC_ALL, "");
		initscr();
		screen_info scr;
		left_pane lp(scr.get_y(), scr.get_x());
		right_pane rp(scr.get_x());
		keybinds kb;
		bool is_running = true;
		timeout(0); //draw window first time instantly
		while (is_running) {
			scr.check_resize(lp.pane, rp.pane, rp.get_draw());
			werase(lp.pane);
			werase(rp.pane);
			lp.update(scr, rp.get_draw(), kb.get_search_str(), flags);
			if (rp.get_draw()) {
				rp.update(scr, lp.get_selected_filepath(), lp.get_size(), flags);
			}
			const wchar_t input = getch();
			switch (input) {
				case 'q':
					is_running = kb.quit(scr);
					break;
				case 'a':
				case 'h':
				case KEY_LEFT:
					kb.move_left(scr);
					break;
				case 'd':
				case 'l':
				case KEY_RIGHT:
					kb.move_right(scr, lp.get_selected_filepath());
					break;
				case 's':
				case 'j':
				case KEY_DOWN:
					kb.move_down(scr, lp.get_files().size(), lp.get_size());
					break;
				case 'n':
					kb.jump_to_bottom(scr, lp.get_files().size());
					break;
				case '-':
				case KEY_PPAGE:
					kb.up_page(scr);
					break;
				case '=':
				case KEY_NPAGE:
					kb.down_page(scr, lp.get_size());
					break;
				case ' ':
					kb.jump_to_line(scr, lp.get_size());
					break;
				case 'u':
					kb.spawn_shell();
					break;
				case 'p':
					kb.paste(scr, lp.get_current_path());
					break;
				case 'b':
					kb.search(scr);
					break;
				case ';':
					kb.screen_change(scr, lp.pane, rp.get_draw());
					break;
				case '?':
					kb.help(scr);
			}
			if (lp.get_files().size() > 0) {
				switch (input) {
					case 'w':
					case 'k':
					case KEY_UP:
						kb.move_up(scr);
						break;
					case 'm':
						scr.set_curs_y(0);
						break;
					case 'x':
						kb.cut(scr, lp.get_selected_filepath());
						break;
					case 'c':
						kb.copy(scr, lp.get_selected_filepath());
						break;
					case 'i':
					case '\n':
						kb.xdg_open(lp.get_selected_filepath());
						break;
					case 'g':
						kb.remove(scr, lp.get_selected_filepath());
						break;
					case 'e':
						kb.rename(scr, lp.get_selected_filepath());
						break;
					case 'v':
						kb.edit_text(lp.get_selected_filepath());
						break;
					case 'y':
						kb.pager(lp.get_selected_filepath());
				}
			}
			timeout(scr.update_speed);
		}
		endwin();
	}
	return 0;
}
