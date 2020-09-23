#include <ncurses.h>
#include <filesystem>
#include <string>
#include <locale.h>
#include "FlagParse.h"
#include "Keybinds.h"
#include "Screen.h"
#include "LeftPane.h"
#include "RightPane.h"

int main(int argc, char *argv[]) {
	FlagParse flags(argc, argv);
	if (flags.get_start_program()) {
		setlocale(LC_ALL, "");
		initscr();
		Screen scr;
		LeftPane lp(scr.get_x(), scr.get_y());
		RightPane rp(scr.get_x(), scr.get_y());
		Keybinds kb;
		bool is_running = true;
		timeout(0); //draw window first time instantly
		while (is_running) {
			scr.check_resize(lp, rp);
			lp.update(scr, rp.get_draw(), kb.get_search_str(), flags);
			rp.update(scr, lp.get_selected_filepath(), lp.get_size(), kb.get_search_str().size(), flags);
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
					rp.set_draw(kb.screen_change(scr, lp, rp.get_draw()));
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
