#ifndef FILE_IO_H
#define FILE_IO_H
#include "user_interface.h"

class file_io {
	public:
		bool contents_printable = true;
		std::string selected_filepath = "?";
		std::string current_path;
		size_t left_pane_size; //size before erasing part of the vector
		std::vector<std::string> left_pane_files;
		std::vector<std::string> right_pane_files;
		file_io(const std::string &current_path,
			       	bool show_hidden_files, const std::string &search_str);
		void set_dir_files(unsigned int page, unsigned int term_height, int curs_y, 
				bool draw_right_pane, bool show_hidden_files);
		inline static std::string path_to_filename(const std::string &path) {
			std::string filename = path.substr(path.find_last_of("/\\") + 1);
			return filename;
		}
		std::vector<std::string> get_file_contents(const std::string &path, unsigned int term_height);
		static std::string get_permbits(const std::string &current_filepath);
		inline bool file_contents_printable(const std::string &path) {
			if (std::filesystem::is_directory(path) ||
					std::filesystem::is_block_file(path) ||
					std::filesystem::is_character_file(path) ||
					std::filesystem::is_fifo(path) ||
					std::filesystem::is_socket(path)) {
				return false;
			}
			return true;
		}
	private:
		std::vector<std::string> get_dir_files(const std::string &path, bool show_hidden_files,
			       	const std::string &search_str = "");
};

#endif
