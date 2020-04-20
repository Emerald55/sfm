CXXFLAGS = -std=c++17 -Wall -Werror -Wextra -O3
LIBS = -lcurses -lstdc++fs
SRCS = src/main.cpp src/flag_parse.cpp src/file_io.cpp src/screen_info.cpp src/input.cpp
SRCS += src/keybinds.cpp src/pane.cpp src/left_pane.cpp src/right_pane.cpp
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
