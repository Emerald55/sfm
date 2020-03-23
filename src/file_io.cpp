#include <string>
#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <filesystem>
#include <ncurses.h>
#include <fstream>
#include <algorithm>
#include "file_io.h"

file_io::file_io(const std::string &current_path,
	       	bool show_hidden_files, const std::string &search_str) {
	this->current_path = current_path;
	current_dir_files = get_dir_files(current_path, show_hidden_files, search_str);
	current_dir_size = current_dir_files.size();
	std::sort(current_dir_files.begin(), current_dir_files.end());
}

std::vector<std::string> file_io::get_dir_files(const std::string &path, bool show_hidden_files, 
		const std::string &search_str) {
	std::vector<std::string> files;
	for (const auto &entry : std::filesystem::directory_iterator(path)) {
		bool was_pushed = false;
		if (show_hidden_files) {
			files.push_back(entry.path());
			was_pushed = true;
		}
		else {
			if (path_to_filename(entry.path().string()).rfind(".", 0) != 0) {
				files.push_back(entry.path());
				was_pushed = true;
			}
		}
		if (was_pushed && !search_str.empty()) {
			if (path_to_filename(entry.path().string()).find(search_str) ==
					std::string::npos) {
				files.pop_back();
			}
		}
	}
	return files;
}

std::vector<std::string> file_io::file_contents(const std::string &path,
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
			if (line.find('\0') != std::string::npos) { //check for null bytes in binary
				contents.clear();
				contents_printable = false;
				break;
			}
			contents.push_back(line);
			if (i > term_height) { break; }
			i++;
		}
	}
	file.close();
	return contents;
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
