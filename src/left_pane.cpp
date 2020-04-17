#include "left_pane.h"
#include "file_io.h"
#include <algorithm>

left_pane::left_pane(int &curs_y, bool show_hidden_files, const std::string &search_str, 
		const std::string &current_path, unsigned int &page, unsigned int term_height) {
	files = file_io::get_dir_files(current_path, show_hidden_files, search_str);
	std::sort(files.begin(), files.end());
	size = files.size();
	const unsigned int current_line_number = (page - term_height) + static_cast<unsigned>(curs_y) + 1;
	if (size < current_line_number) { //if cursor was over a file and it was deleted
		curs_y = 0;
		page = term_height;
	}
	if (size != 0) {
		const int page_floor = page - term_height;
		files.erase(files.begin(), files.begin() + page_floor); //trim before page
		if (size > page) {
			files.erase(files.begin() + page - page_floor, files.end()); //trim after
		}
		selected_filepath = files[curs_y];
	}
}
