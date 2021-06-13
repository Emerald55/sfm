#include <ncurses.h>
#include <chrono>
#include <thread>
#include <string>
#include <cstring>
#include "Input.h"
#include "Screen.h"

std::string Input::input_box(const std::string &text, unsigned int color_type, 
		const Screen &scr, unsigned int win_width) {
	WINDOW *input_text_win = newwin(1, text.size(), scr.get_y() - 1, 2);
	wattron(input_text_win, COLOR_PAIR(color_type));
	waddstr(input_text_win, text.c_str());
	wattroff(input_text_win, COLOR_PAIR(color_type));
	wrefresh(input_text_win);
	WINDOW *input_win = newwin(1, win_width, scr.get_y() - 1, 2 + text.size());
	curs_set(1);
	scrollok(input_win, 1);
	echo();
	nocbreak();
	std::string user_input;
	while (const char input_char = wgetch(input_win)) { //get input a char at a time
		if (input_char == ERR || input_char == '\n') {
			break;
		}
		user_input += input_char;
	}
	cbreak();
	noecho();
	wrefresh(input_win);
	curs_set(0);
	delwin(input_win);
	delwin(input_text_win);
	return user_input;
}

void Input::alert_box(const char* text, unsigned int sleep_time, 
		unsigned int alert_color, const Screen &scr) {
	WINDOW *alert_win = newwin(1, strlen(text), scr.get_y() - 1, 2);
	wattron(alert_win, COLOR_PAIR(alert_color));
	mvwaddstr(alert_win, 0, 0, text);
	wattroff(alert_win, COLOR_PAIR(alert_color));
	wrefresh(alert_win);
	std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
	delwin(alert_win);
}
