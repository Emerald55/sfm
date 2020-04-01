#include "flag_parse.h"
#include <getopt.h>
#include <unistd.h>
#include <iostream>

bool parse_flags(bool &show_symbolic_links, bool &show_hidden_files, int argc, char* argv[]) {
	const option long_opts[] = {
		{"help", no_argument, nullptr, 'h'},
		{"all", no_argument, nullptr, 'a'},
		{"symlinks", no_argument, nullptr, 's'},
		{"path", required_argument, nullptr, 'p'}
	};
	int opt;
	while((opt = getopt_long(argc, argv, ":p:has", long_opts, nullptr)) != -1) {
		switch (opt) {
			case '?':
			       std::cerr << "Invalid flag, type \"-h\" for options.\n";
			       return false;
			case ':':
			       std::cerr << "A flag is missing a required argument.\n";
			       return false;
			case 'a':
			       show_hidden_files = true;
			       break;
			case 's':
			       show_symbolic_links = true;
			       break;
			case 'h':
				std::cout << "Sfm Usage:\n";
				std::cout << "	-h --help              Displays help options\n";
				std::cout << "	-a --all               Unhides files starting with a \".\"\n";
				std::cout << "	-s --symlinks          Shows symlinks\n";
				std::cout << "	-p --path /start/path  Path to start program from\n";
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
