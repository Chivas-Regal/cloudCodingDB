# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/home/parallels/Desktop/Parallels Shared Folders/Home/Desktop/memsocket/mem_socket_server"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/home/parallels/Desktop/Parallels Shared Folders/Home/Desktop/memsocket/mem_socket_server/build"

# Include any dependencies generated for this target.
include CMakeFiles/memserverlib.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/memserverlib.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/memserverlib.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/memserverlib.dir/flags.make

CMakeFiles/memserverlib.dir/example/main.cpp.o: CMakeFiles/memserverlib.dir/flags.make
CMakeFiles/memserverlib.dir/example/main.cpp.o: ../example/main.cpp
CMakeFiles/memserverlib.dir/example/main.cpp.o: CMakeFiles/memserverlib.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/parallels/Desktop/Parallels Shared Folders/Home/Desktop/memsocket/mem_socket_server/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/memserverlib.dir/example/main.cpp.o"
	/usr/bin/aarch64-linux-gnu-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/memserverlib.dir/example/main.cpp.o -MF CMakeFiles/memserverlib.dir/example/main.cpp.o.d -o CMakeFiles/memserverlib.dir/example/main.cpp.o -c "/home/parallels/Desktop/Parallels Shared Folders/Home/Desktop/memsocket/mem_socket_server/example/main.cpp"

CMakeFiles/memserverlib.dir/example/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/memserverlib.dir/example/main.cpp.i"
	/usr/bin/aarch64-linux-gnu-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/home/parallels/Desktop/Parallels Shared Folders/Home/Desktop/memsocket/mem_socket_server/example/main.cpp" > CMakeFiles/memserverlib.dir/example/main.cpp.i

CMakeFiles/memserverlib.dir/example/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/memserverlib.dir/example/main.cpp.s"
	/usr/bin/aarch64-linux-gnu-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/home/parallels/Desktop/Parallels Shared Folders/Home/Desktop/memsocket/mem_socket_server/example/main.cpp" -o CMakeFiles/memserverlib.dir/example/main.cpp.s

# Object files for target memserverlib
memserverlib_OBJECTS = \
"CMakeFiles/memserverlib.dir/example/main.cpp.o"

# External object files for target memserverlib
memserverlib_EXTERNAL_OBJECTS =

memserverlib: CMakeFiles/memserverlib.dir/example/main.cpp.o
memserverlib: CMakeFiles/memserverlib.dir/build.make
memserverlib: ../lib/libmem_socket_server.so
memserverlib: CMakeFiles/memserverlib.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/home/parallels/Desktop/Parallels Shared Folders/Home/Desktop/memsocket/mem_socket_server/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable memserverlib"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/memserverlib.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/memserverlib.dir/build: memserverlib
.PHONY : CMakeFiles/memserverlib.dir/build

CMakeFiles/memserverlib.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/memserverlib.dir/cmake_clean.cmake
.PHONY : CMakeFiles/memserverlib.dir/clean

CMakeFiles/memserverlib.dir/depend:
	cd "/home/parallels/Desktop/Parallels Shared Folders/Home/Desktop/memsocket/mem_socket_server/build" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/home/parallels/Desktop/Parallels Shared Folders/Home/Desktop/memsocket/mem_socket_server" "/home/parallels/Desktop/Parallels Shared Folders/Home/Desktop/memsocket/mem_socket_server" "/home/parallels/Desktop/Parallels Shared Folders/Home/Desktop/memsocket/mem_socket_server/build" "/home/parallels/Desktop/Parallels Shared Folders/Home/Desktop/memsocket/mem_socket_server/build" "/home/parallels/Desktop/Parallels Shared Folders/Home/Desktop/memsocket/mem_socket_server/build/CMakeFiles/memserverlib.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/memserverlib.dir/depend

