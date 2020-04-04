#ifndef RIGHT_PANE_H
#define RIGHT_PANE_H
#include <vector>
#include <string>

class right_pane {
	public:
		std::vector<std::string> files;
		std::vector<std::string> file_content;
		right_pane(const std::string &selected_filepath, bool show_hidden_files, 
				bool draw_right_pane, size_t left_pane_size,
			       	unsigned int term_height);
};

#endif
