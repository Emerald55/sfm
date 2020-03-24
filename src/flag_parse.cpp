#include "flag_parse.h"
#include <unistd.h>
#include <iostream>

bool parse_flags(bool &show_symbolic_links, bool &show_hidden_files, int argc, char* argv[]) {
	int opt;
	while((opt = getopt(argc, argv, ":p:has")) != -1) {
		switch (opt) {
			case '?':
			       std::cerr << "Invalid flag, type \"-h\" for options.\n";
			       return false;
			case ':':
			       std::cerr << "A flag is missing an extra value.\n";
			       return false;
			case 'a':
			       show_hidden_files = true;
			       break;
			case 's':
			       show_symbolic_links = true;
			       break;
			case 'h':
				std::cout << "Sfm Usage:\n";
				std::cout << "	-h                Displays help options\n";
				std::cout << "	-a                Unhides files starting with a \".\"\n";
				std::cout << "	-s                Shows symlinks\n";
				std::cout << "	-p /start/path    Path to start program from\n";
				return false;
			case 'p':
				errno = chdir(optarg);
				if (errno == -1) {
					std::cerr << "Could Not Find And/Or Enter Directory, Exiting...\n";
					return false;
				}
		}
	}
	return true;
}
