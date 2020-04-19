#include "left_pane.h"
#include "file_io.h"
#include <algorithm>
#include <math.h>

left_pane::left_pane(unsigned int &curs_y, bool show_hidden_files, const std::string &search_str, 
		const std::string &current_path, unsigned int &page, unsigned int term_height) {
	files = file_io::get_dir_files(current_path, show_hidden_files, search_str);
	std::sort(files.begin(), files.end());
	size = files.size();
	const unsigned int current_line_number = (page - term_height) + curs_y + 1;
	if (size < current_line_number) { //if cursor was over a file and it was deleted
		page = static_cast<unsigned int>(std::ceil(static_cast<double>(size)
					/ static_cast<double>(term_height))) * term_height;
	}
	if (size != 0) {
		const unsigned int page_floor = page - term_height;
		files.erase(files.begin(), files.begin() + page_floor); //trim before page
		if (size > page) {
			files.erase(files.begin() + page - page_floor, files.end()); //trim after
		}
	}
	if (size < current_line_number) { //set cursor to last available file after trim
		curs_y = files.size() - 1;
	}
	if (size != 0) {
		selected_filepath = files[curs_y];
	}
}
