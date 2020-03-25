#ifndef LEFT_PANE_H
#define LEFT_PANE_H
#include <vector>
#include <string>

class left_pane {
	public:
		std::vector<std::string> files;
		size_t size;
		std::string selected_filepath = "?";
		left_pane(int curs_y, bool show_hidden_files, const std::string &search_str,
			       	const std::string &current_path, unsigned int page,
			       	unsigned int term_height);
};

#endif
