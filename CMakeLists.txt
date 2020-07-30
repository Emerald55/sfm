cmake_minimum_required(VERSION 3.0)
project(sfm LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "-Wall -Werror -Wextra -O3")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_RELEASE} -fsanitize=address -fsanitize=undefined -g")
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})
add_executable(
   "${PROJECT_NAME}"
   src/main.cpp
   src/FlagParse.cpp
   src/FileIO.cpp
   src/Screen.cpp
   src/Input.cpp
   src/Keybinds.cpp
   src/Pane.cpp
   src/LeftPane.cpp
   src/RightPane.cpp
)
target_link_libraries("${PROJECT_NAME}" ${CURSES_LIBRARIES} stdc++fs)

set(CMAKE_INSTALL_PREFIX /usr/local/bin)
install(TARGETS "${PROJECT_NAME}" DESTINATION ${CMAKE_INSTALL_PREFIX})
