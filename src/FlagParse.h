#ifndef FLAGPARSE_H
#define FLAGPARSE_H

class FlagParse {
	public:
		FlagParse(int argc, char* argv[]);
		inline bool get_start_program() const { return start_program; }
		inline bool get_show_symbolic_links() const { return show_symbolic_links; }
		inline bool get_show_hidden_files() const { return show_hidden_files; }
	private:
		bool parse_flags(int argc, char* argv[]);
		bool start_program = false;
		bool show_symbolic_links = false;
		bool show_hidden_files = false;
};

#endif
