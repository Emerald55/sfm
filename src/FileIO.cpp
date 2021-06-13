#include <string>
#include <vector>
#include <sys/stat.h>
#include <pwd.h>
#include <filesystem>
#include <ncurses.h>
#include <fstream>
#include <algorithm>
#include "FileIO.h"
#include "FlagParse.h"

std::vector<std::string> FileIO::get_file_contents(const std::string &path,
	       	unsigned int term_height) {
	std::ifstream file(path);
	std::vector<std::string> contents;
	if (file) {
		std::string line;
		size_t i = 0;
		while (std::getline(file, line)) {
			size_t pos;
			while ((pos = line.find("\t")) != std::string::npos) {
				line.replace(pos, 1, "    "); //convert tabs to space for counting
			}
			contents.push_back(line);
			if (i >= term_height - 1) { break; }
			i++;
		}
	}
	file.close();
	return contents;
}

std::vector<std::string> FileIO::get_dir_files(const std::string &path, const FlagParse &flags, 
		const std::string &search_str) {
	std::vector<std::string> files;
	for (const auto &entry : std::filesystem::directory_iterator(path,
			std::filesystem::directory_options::skip_permission_denied)) {
		if (flags.get_show_hidden_files()) {
			if (entry.path().filename().string().find(search_str) !=
					std::string::npos) {
				files.push_back(entry.path());
			}
		}
		else if (entry.path().filename().string().rfind(".", 0) != 0) {
			if (entry.path().filename().string().find(search_str) !=
					std::string::npos) {
				files.push_back(entry.path());
			}
		}
	}
	return files;
}

std::string FileIO::get_permbits(const std::string &current_filepath) {
	struct stat st;
	if (stat(current_filepath.c_str(), &st)) {
		return " ? ";
	}
	const std::filesystem::perms fp = std::filesystem::status(current_filepath).permissions();
	std::string file_perms = " ";
	file_perms += (fp & std::filesystem::perms::owner_read) != std::filesystem::perms::none ? 'r' : '-';
	file_perms += (fp & std::filesystem::perms::owner_write) != std::filesystem::perms::none ? 'w' : '-';
	file_perms += (fp & std::filesystem::perms::owner_exec) != std::filesystem::perms::none ? 'x' : '-';
	file_perms += (fp & std::filesystem::perms::group_read) != std::filesystem::perms::none ? 'r' : '-';
	file_perms += (fp & std::filesystem::perms::group_write) != std::filesystem::perms::none ? 'w' : '-';
	file_perms += (fp & std::filesystem::perms::group_exec) != std::filesystem::perms::none ? 'x' : '-';
	file_perms += (fp & std::filesystem::perms::others_read) != std::filesystem::perms::none ? 'r' : '-';
	file_perms += (fp & std::filesystem::perms::others_write) != std::filesystem::perms::none ? 'w' : '-';
	file_perms += (fp & std::filesystem::perms::others_exec) != std::filesystem::perms::none ? 'x' : '-';
	const struct passwd *pwd = getpwuid(st.st_uid); //get file owner
	return file_perms + " " + pwd->pw_name + " ";
}
