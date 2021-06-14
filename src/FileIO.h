#ifndef FILEIO_H
#define FILEIO_H
#include <string>
#include <vector>
#include "FlagParse.h"

struct FileIO {
	static std::vector<std::string> get_file_contents(const std::string &path, unsigned int term_height);
	static std::vector<std::string> get_dir_files(const std::string &path, const FlagParse &flags,
			const std::string &search_str = "");
	static std::string get_permbits(const std::string &current_filepath);
};

#endif
