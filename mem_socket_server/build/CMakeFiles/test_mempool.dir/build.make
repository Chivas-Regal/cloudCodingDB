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
CMAKE_SOURCE_DIR = /home/projects/cloudCodingDB/mem_socket_server

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/projects/cloudCodingDB/mem_socket_server/build

# Include any dependencies generated for this target.
include CMakeFiles/test_mempool.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/test_mempool.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/test_mempool.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test_mempool.dir/flags.make

CMakeFiles/test_mempool.dir/test/mempool.cpp.o: CMakeFiles/test_mempool.dir/flags.make
CMakeFiles/test_mempool.dir/test/mempool.cpp.o: ../test/mempool.cpp
CMakeFiles/test_mempool.dir/test/mempool.cpp.o: CMakeFiles/test_mempool.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/projects/cloudCodingDB/mem_socket_server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/test_mempool.dir/test/mempool.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/test_mempool.dir/test/mempool.cpp.o -MF CMakeFiles/test_mempool.dir/test/mempool.cpp.o.d -o CMakeFiles/test_mempool.dir/test/mempool.cpp.o -c /home/projects/cloudCodingDB/mem_socket_server/test/mempool.cpp

CMakeFiles/test_mempool.dir/test/mempool.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_mempool.dir/test/mempool.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/projects/cloudCodingDB/mem_socket_server/test/mempool.cpp > CMakeFiles/test_mempool.dir/test/mempool.cpp.i

CMakeFiles/test_mempool.dir/test/mempool.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_mempool.dir/test/mempool.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/projects/cloudCodingDB/mem_socket_server/test/mempool.cpp -o CMakeFiles/test_mempool.dir/test/mempool.cpp.s

# Object files for target test_mempool
test_mempool_OBJECTS = \
"CMakeFiles/test_mempool.dir/test/mempool.cpp.o"

# External object files for target test_mempool
test_mempool_EXTERNAL_OBJECTS =

test_mempool: CMakeFiles/test_mempool.dir/test/mempool.cpp.o
test_mempool: CMakeFiles/test_mempool.dir/build.make
test_mempool: ../lib/libmem_socket_server.so
test_mempool: CMakeFiles/test_mempool.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/projects/cloudCodingDB/mem_socket_server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable test_mempool"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_mempool.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test_mempool.dir/build: test_mempool
.PHONY : CMakeFiles/test_mempool.dir/build

CMakeFiles/test_mempool.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test_mempool.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test_mempool.dir/clean

CMakeFiles/test_mempool.dir/depend:
	cd /home/projects/cloudCodingDB/mem_socket_server/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/projects/cloudCodingDB/mem_socket_server /home/projects/cloudCodingDB/mem_socket_server /home/projects/cloudCodingDB/mem_socket_server/build /home/projects/cloudCodingDB/mem_socket_server/build /home/projects/cloudCodingDB/mem_socket_server/build/CMakeFiles/test_mempool.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/test_mempool.dir/depend

