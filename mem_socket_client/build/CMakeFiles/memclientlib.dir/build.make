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
CMAKE_SOURCE_DIR = /root/projects/cloudCodingDB/mem_socket_client

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /root/projects/cloudCodingDB/mem_socket_client/build

# Include any dependencies generated for this target.
include CMakeFiles/memclientlib.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/memclientlib.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/memclientlib.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/memclientlib.dir/flags.make

CMakeFiles/memclientlib.dir/example/main.cpp.o: CMakeFiles/memclientlib.dir/flags.make
CMakeFiles/memclientlib.dir/example/main.cpp.o: ../example/main.cpp
CMakeFiles/memclientlib.dir/example/main.cpp.o: CMakeFiles/memclientlib.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/projects/cloudCodingDB/mem_socket_client/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/memclientlib.dir/example/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/memclientlib.dir/example/main.cpp.o -MF CMakeFiles/memclientlib.dir/example/main.cpp.o.d -o CMakeFiles/memclientlib.dir/example/main.cpp.o -c /root/projects/cloudCodingDB/mem_socket_client/example/main.cpp

CMakeFiles/memclientlib.dir/example/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/memclientlib.dir/example/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/projects/cloudCodingDB/mem_socket_client/example/main.cpp > CMakeFiles/memclientlib.dir/example/main.cpp.i

CMakeFiles/memclientlib.dir/example/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/memclientlib.dir/example/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/projects/cloudCodingDB/mem_socket_client/example/main.cpp -o CMakeFiles/memclientlib.dir/example/main.cpp.s

# Object files for target memclientlib
memclientlib_OBJECTS = \
"CMakeFiles/memclientlib.dir/example/main.cpp.o"

# External object files for target memclientlib
memclientlib_EXTERNAL_OBJECTS =

memclientlib: CMakeFiles/memclientlib.dir/example/main.cpp.o
memclientlib: CMakeFiles/memclientlib.dir/build.make
memclientlib: ../lib/libmem_socket_client.so
memclientlib: CMakeFiles/memclientlib.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/root/projects/cloudCodingDB/mem_socket_client/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable memclientlib"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/memclientlib.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/memclientlib.dir/build: memclientlib
.PHONY : CMakeFiles/memclientlib.dir/build

CMakeFiles/memclientlib.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/memclientlib.dir/cmake_clean.cmake
.PHONY : CMakeFiles/memclientlib.dir/clean

CMakeFiles/memclientlib.dir/depend:
	cd /root/projects/cloudCodingDB/mem_socket_client/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/projects/cloudCodingDB/mem_socket_client /root/projects/cloudCodingDB/mem_socket_client /root/projects/cloudCodingDB/mem_socket_client/build /root/projects/cloudCodingDB/mem_socket_client/build /root/projects/cloudCodingDB/mem_socket_client/build/CMakeFiles/memclientlib.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/memclientlib.dir/depend

