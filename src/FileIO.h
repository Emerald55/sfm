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
		try {
			switch (std::filesystem::status(path).type()) {
				case std::filesystem::file_type::none:
				case std::filesystem::file_type::not_found:
				case std::filesystem::file_type::unknown:
				case std::filesystem::file_type::directory:
				case std::filesystem::file_type::block:
				case std::filesystem::file_type::character:
				case std::filesystem::file_type::fifo:
				case std::filesystem::file_type::socket:
					return false;
				default:
					return true;
			}
		}
		catch (const std::filesystem::filesystem_error &) {
			return false;
		}
	}
};

#endif
