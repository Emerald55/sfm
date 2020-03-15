#ifndef FILE_IO_H
#define FILE_IO_H
#include "user_interface.h"

bool check_for_flag(int argc, char* argv[], const std::string &flag);

class file_io {
	public:
		std::string selected_filepath = "?";
		std::string current_path;
		std::vector<std::string> current_dir_files; //left window
		size_t current_dir_size; //size before erasing part of the vector
		std::vector<std::string> selected_dir_files; //right window
		file_io(const std::string &current_path,
			       	int argc, char* argv[], const std::string &search_str);
		inline static std::string path_to_filename(const std::string &path) {
			std::string filename = path.substr(path.find_last_of("/\\") + 1);
			return filename;
		}
		std::vector<std::string> get_dir_files(const std::string &path, int argc,
			       	char* argv[], const std::string &search_str = "");
		std::vector<std::string> file_contents(const std::string &path, unsigned int term_height);
		static std::string get_permbits(const std::string &current_filepath);
};

#endif
