#ifndef FILE_IO_H
#define FILE_IO_H
#include "user_interface.h"
#include <filesystem>

class file_io {
	public:
		inline static std::string path_to_filename(const std::string &path) {
			std::string filename = path.substr(path.find_last_of("/\\") + 1);
			return filename;
		}
		static std::vector<std::string> get_file_contents(const std::string &path, unsigned int term_height);
		static std::vector<std::string> get_dir_files(const std::string &path, bool show_hidden_files,
			       	const std::string &search_str = "");
		static std::string get_permbits(const std::string &current_filepath);
		inline static bool file_contents_printable(const std::string &path) {
			if (std::filesystem::is_directory(path) ||
					std::filesystem::is_block_file(path) ||
					std::filesystem::is_character_file(path) ||
					std::filesystem::is_fifo(path) ||
					std::filesystem::is_socket(path)) {
				return false;
			}
			return true;
		}
};

#endif
