# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/sunshine/Documents/compile/PW2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/sunshine/Documents/compile/PW2/build

# Include any dependencies generated for this target.
include CMakeFiles/PW2.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/PW2.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/PW2.dir/flags.make

CMakeFiles/PW2.dir/PW2.c.o: CMakeFiles/PW2.dir/flags.make
CMakeFiles/PW2.dir/PW2.c.o: ../PW2.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/sunshine/Documents/compile/PW2/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/PW2.dir/PW2.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/PW2.dir/PW2.c.o   -c /home/sunshine/Documents/compile/PW2/PW2.c

CMakeFiles/PW2.dir/PW2.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/PW2.dir/PW2.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/sunshine/Documents/compile/PW2/PW2.c > CMakeFiles/PW2.dir/PW2.c.i

CMakeFiles/PW2.dir/PW2.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/PW2.dir/PW2.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/sunshine/Documents/compile/PW2/PW2.c -o CMakeFiles/PW2.dir/PW2.c.s

# Object files for target PW2
PW2_OBJECTS = \
"CMakeFiles/PW2.dir/PW2.c.o"

# External object files for target PW2
PW2_EXTERNAL_OBJECTS =

PW2: CMakeFiles/PW2.dir/PW2.c.o
PW2: CMakeFiles/PW2.dir/build.make
PW2: CMakeFiles/PW2.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/sunshine/Documents/compile/PW2/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable PW2"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/PW2.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/PW2.dir/build: PW2

.PHONY : CMakeFiles/PW2.dir/build

CMakeFiles/PW2.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/PW2.dir/cmake_clean.cmake
.PHONY : CMakeFiles/PW2.dir/clean

CMakeFiles/PW2.dir/depend:
	cd /home/sunshine/Documents/compile/PW2/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/sunshine/Documents/compile/PW2 /home/sunshine/Documents/compile/PW2 /home/sunshine/Documents/compile/PW2/build /home/sunshine/Documents/compile/PW2/build /home/sunshine/Documents/compile/PW2/build/CMakeFiles/PW2.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/PW2.dir/depend

