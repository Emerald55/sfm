#include "right_pane.h"
#include "file_io.h"
#include <algorithm>

right_pane::right_pane(const std::string &selected_filepath, bool show_hidden_files, 
		bool draw_right_pane, size_t left_pane_size) {
	if (left_pane_size != 0) {
		try {
			if (std::filesystem::is_directory(selected_filepath) && draw_right_pane) {
				files = file_io::get_dir_files(selected_filepath, show_hidden_files);
				std::sort(files.begin(), files.end());
			
			}
		} catch (const std::filesystem::filesystem_error &) {} //no permission to read contents
	}
}
