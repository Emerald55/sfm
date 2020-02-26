#ifndef FILE_IO_H
#define FILE_IO_H
#include "user_interface.h"

bool check_for_flag(int argc, char* argv[], const std::string &flag);

class file_io {
	public:
		inline static std::string path_to_filename(const std::string &path) {
			std::string filename = path.substr(path.find_last_of("/\\") + 1);
			return filename;
		}
		static std::vector<std::string> get_dir_files(const std::string &path, const int &argc,
			       	char* argv[], const std::string &search_str = "");
		static std::vector<std::string> file_contents(const std::string &path, const unsigned int &term_height);
		static std::string get_permbits(const std::string &current_filename);
};

#endif
