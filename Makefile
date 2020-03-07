all:
	g++ src/main.cpp src/user_interface.cpp src/file_io.cpp src/keybinds.cpp -o sfm -lcurses -lstdc++fs -std=c++17 -Wall -Werror -Wextra -O3
debug:
	g++ src/main.cpp src/user_interface.cpp src/file_io.cpp src/keybinds.cpp -o sfm -lcurses -lstdc++fs -std=c++17 -Wall -Werror -Wextra -O3 -fsanitize=address -g
install:
	g++ src/main.cpp src/user_interface.cpp src/file_io.cpp src/keybinds.cpp -o sfm -lcurses -lstdc++fs -std=c++17 -Wall -Werror -Wextra -O3
	mkdir -p /usr/local/bin/
	mv sfm /usr/local/bin/
	chmod 755 /usr/local/bin/sfm
uninstall:
	rm /usr/local/bin/sfm
