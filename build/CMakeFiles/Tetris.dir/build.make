# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.15.3/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.15.3/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/AnandTyagi/Desktop/Tetris

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/AnandTyagi/Desktop/Tetris/build

# Include any dependencies generated for this target.
include CMakeFiles/Tetris.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Tetris.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Tetris.dir/flags.make

CMakeFiles/Tetris.dir/src/main.cpp.o: CMakeFiles/Tetris.dir/flags.make
CMakeFiles/Tetris.dir/src/main.cpp.o: ../src/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/AnandTyagi/Desktop/Tetris/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Tetris.dir/src/main.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Tetris.dir/src/main.cpp.o -c /Users/AnandTyagi/Desktop/Tetris/src/main.cpp

CMakeFiles/Tetris.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Tetris.dir/src/main.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/AnandTyagi/Desktop/Tetris/src/main.cpp > CMakeFiles/Tetris.dir/src/main.cpp.i

CMakeFiles/Tetris.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Tetris.dir/src/main.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/AnandTyagi/Desktop/Tetris/src/main.cpp -o CMakeFiles/Tetris.dir/src/main.cpp.s

CMakeFiles/Tetris.dir/src/helpers.cpp.o: CMakeFiles/Tetris.dir/flags.make
CMakeFiles/Tetris.dir/src/helpers.cpp.o: ../src/helpers.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/AnandTyagi/Desktop/Tetris/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/Tetris.dir/src/helpers.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Tetris.dir/src/helpers.cpp.o -c /Users/AnandTyagi/Desktop/Tetris/src/helpers.cpp

CMakeFiles/Tetris.dir/src/helpers.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Tetris.dir/src/helpers.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/AnandTyagi/Desktop/Tetris/src/helpers.cpp > CMakeFiles/Tetris.dir/src/helpers.cpp.i

CMakeFiles/Tetris.dir/src/helpers.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Tetris.dir/src/helpers.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/AnandTyagi/Desktop/Tetris/src/helpers.cpp -o CMakeFiles/Tetris.dir/src/helpers.cpp.s

# Object files for target Tetris
Tetris_OBJECTS = \
"CMakeFiles/Tetris.dir/src/main.cpp.o" \
"CMakeFiles/Tetris.dir/src/helpers.cpp.o"

# External object files for target Tetris
Tetris_EXTERNAL_OBJECTS =

Tetris: CMakeFiles/Tetris.dir/src/main.cpp.o
Tetris: CMakeFiles/Tetris.dir/src/helpers.cpp.o
Tetris: CMakeFiles/Tetris.dir/build.make
Tetris: glfw/src/libglfw3.a
Tetris: glad/libglad.a
Tetris: CMakeFiles/Tetris.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/AnandTyagi/Desktop/Tetris/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable Tetris"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Tetris.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Tetris.dir/build: Tetris

.PHONY : CMakeFiles/Tetris.dir/build

CMakeFiles/Tetris.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Tetris.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Tetris.dir/clean

CMakeFiles/Tetris.dir/depend:
	cd /Users/AnandTyagi/Desktop/Tetris/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/AnandTyagi/Desktop/Tetris /Users/AnandTyagi/Desktop/Tetris /Users/AnandTyagi/Desktop/Tetris/build /Users/AnandTyagi/Desktop/Tetris/build /Users/AnandTyagi/Desktop/Tetris/build/CMakeFiles/Tetris.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Tetris.dir/depend

