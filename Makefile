CXXFLAGS = -std=c++17 -Wall -Werror -Wextra -O3
LIBS = -lcurses -lstdc++fs
SRCS = src/main.cpp src/user_interface.cpp src/file_io.cpp
SRCS += src/keybinds.cpp src/flag_parse.cpp src/left_pane.cpp src/right_pane.cpp
TARGET = sfm
all:
	g++ $(SRCS) -o $(TARGET) $(LIBS) $(CXXFLAGS)
debug:
	g++ $(SRCS) -o $(TARGET) $(LIBS) $(CXXFLAGS) -fsanitize=address -g
install:
	g++ $(SRCS) -o $(TARGET) $(LIBS) $(CXXFLAGS)
	mkdir -p /usr/local/bin/
	mv sfm /usr/local/bin/
	chmod 755 /usr/local/bin/sfm
uninstall:
	rm /usr/local/bin/sfm
