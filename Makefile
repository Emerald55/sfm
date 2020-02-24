all:
	g++ src/main.cpp src/user_interface.cpp src/file_io.cpp -o sfm -lcurses -std=c++17 -Wall -Werror -Wextra -fsanitize=address
install:
	g++ src/main.cpp src/user_interface.cpp src/file_io.cpp -o sfm -lcurses -std=c++17 -Wall -Werror -Wextra -fsanitize=address
	mkdir -p /usr/local/bin/
	mv sfm /usr/local/bin/
	chmod 755 /usr/local/bin/sfm
uninstall:
	rm /usr/local/bin/sfm
