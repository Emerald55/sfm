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
	if (flags.start_program) {
		setlocale(LC_ALL, "");
		initscr();
		screen_info scr;
		left_pane lp(scr.y, scr.x);
		right_pane rp(scr.x);
		keybinds kb;
		bool is_running = true;
		timeout(0); //draw window first time instantly
		while (is_running) {
			const std::string current_path = std::filesystem::current_path().string();
			scr.check_resize(lp.pane, rp.pane, rp.draw);
			werase(lp.pane);
			werase(rp.pane);
			lp.update(scr, rp.draw, kb.search_str, current_path, flags);
			if (rp.draw) {
				rp.update(scr, lp.selected_filepath, lp.size, flags);
			}
			wchar_t input = getch();
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
					kb.move_right(scr, lp.selected_filepath);
					break;
				case 's':
				case 'j':
				case KEY_DOWN:
					kb.move_down(scr, lp.files.size(), lp.size);
					break;
				case 'n':
					kb.jump_to_bottom(scr, lp.files.size());
					break;
				case '-':
					kb.up_page(scr);
					break;
				case '=':
					kb.down_page(scr, lp.size);
					break;
				case ' ':
					kb.jump_to_line(scr, lp.size);
					break;
				case 'u':
					kb.spawn_shell();
					break;
				case 'p':
					kb.paste(scr, current_path);
					break;
				case 'b':
					kb.search(scr);
					break;
				case ';':
					kb.screen_change(scr, lp.pane, rp.draw);
					break;
				case '?':
					kb.help(scr);
			}
			if (lp.files.size() > 0) {
				switch (input) {
					case 'w':
					case 'k':
					case KEY_UP:
						kb.move_up(scr);
						break;
					case 'm':
						scr.curs_y = 0;
						break;
					case 'x':
						kb.cut(scr, lp.selected_filepath);
						break;
					case 'c':
						kb.copy(scr, lp.selected_filepath);
						break;
					case 'i':
					case '\n':
						kb.xdg_open(lp.selected_filepath);
						break;
					case 'g':
						kb.remove(scr, lp.selected_filepath);
						break;
					case 'e':
						kb.rename(scr, lp.selected_filepath);
						break;
					case 'v':
						kb.edit_text(lp.selected_filepath);
						break;
					case 'y':
						kb.pager(lp.selected_filepath);
				}
			}
			timeout(scr.update_speed);
		}
		endwin();
	}
	return 0;
}
