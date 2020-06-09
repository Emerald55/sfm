#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <filesystem>
#include <ncurses.h>
#include <fstream>
#include <algorithm>
#include "file_io.h"
#include "flag_parse.h"

std::vector<std::string> file_io::get_file_contents(const std::string &path,
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

std::vector<std::string> file_io::get_dir_files(const std::string &path, const flag_parse &flags, 
		const std::string &search_str) {
	std::vector<std::string> files;
	for (const auto &entry : std::filesystem::directory_iterator(path)) {
		if (flags.show_hidden_files) {
			if (path_to_filename(entry.path().string()).find(search_str) !=
					std::string::npos) {
				files.push_back(entry.path());
			}
		}
		else if (path_to_filename(entry.path().string()).rfind(".", 0) != 0) {
			if (path_to_filename(entry.path().string()).find(search_str) !=
					std::string::npos) {
				files.push_back(entry.path());
			}
		}
	}
	return files;
}

std::string file_io::get_permbits(const std::string &current_filepath) {
	struct stat st;
	std::string file_perms = " ";
	if (stat(current_filepath.c_str(), &st) == 0) {
		mode_t perm = st.st_mode;
		file_perms += (perm & S_IRUSR) ? 'r' : '-';
		file_perms += (perm & S_IWUSR) ? 'w' : '-';
		file_perms += (perm & S_IXUSR) ? 'x' : '-';
		file_perms += (perm & S_IRGRP) ? 'r' : '-';
		file_perms += (perm & S_IWGRP) ? 'w' : '-';
		file_perms += (perm & S_IXGRP) ? 'x' : '-';
		file_perms += (perm & S_IROTH) ? 'r' : '-';
		file_perms += (perm & S_IWOTH) ? 'w' : '-';
		file_perms += (perm & S_IXOTH) ? 'x' : '-';
		stat(current_filepath.c_str(), &st);
		struct passwd *pwd;
		pwd = getpwuid(st.st_uid);
		file_perms = file_perms + " " + pwd->pw_name + " ";
	}
	return file_perms;
}
