#ifndef INPUT_H
#define INPUT_H
#include <string>
#include "screen_info.h"

struct input {
	static std::string input_box(const std::string &text, unsigned int color_type,
		       	const screen_info &scr, unsigned int win_width = 15);
	static void alert_box(const char* text, unsigned int win_width, unsigned int sleep_time,
			unsigned int alert_color, const screen_info &scr);
};

#endif
