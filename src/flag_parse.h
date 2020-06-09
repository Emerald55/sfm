#ifndef FLAG_PARSE_H
#define FLAG_PARSE_H

class flag_parse {
	public:
		bool start_program = false;
		bool show_symbolic_links = false;
		bool show_hidden_files = false;
		flag_parse(int argc, char* argv[]);
	private:
		bool parse_flags(int argc, char* argv[]);
};

#endif
