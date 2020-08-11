#ifndef FILEIO_H
#define FILEIO_H
#include <filesystem>
#include <string>
#include <vector>
#include "FlagParse.h"

struct FileIO {
	static std::vector<std::string> get_file_contents(const std::string &path, unsigned int term_height);
	static std::vector<std::string> get_dir_files(const std::string &path, const FlagParse &flags,
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
