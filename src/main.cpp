#include <ncurses.h>
#include <locale.h>
#include "FlagParse.h"
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
		bool is_running = true;
		timeout(0); //draw window first time instantly
		do {
			scr.check_resize(lp, rp);
			lp.update(scr, rp.get_draw(), flags);
			rp.update(scr, lp.get_selected_filepath(), lp.get_size(), lp.get_searching(), flags);
			is_running = lp.get_input(scr, rp);
		} while (is_running);
		endwin();
	}
	return 0;
}
