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
CMAKE_SOURCE_DIR = /home/projects/cloudCodingDB/mem_socket_client

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/projects/cloudCodingDB/mem_socket_client/build

# Include any dependencies generated for this target.
include CMakeFiles/mem_socket_client.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/mem_socket_client.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/mem_socket_client.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/mem_socket_client.dir/flags.make

CMakeFiles/mem_socket_client.dir/src/connector.cpp.o: CMakeFiles/mem_socket_client.dir/flags.make
CMakeFiles/mem_socket_client.dir/src/connector.cpp.o: ../src/connector.cpp
CMakeFiles/mem_socket_client.dir/src/connector.cpp.o: CMakeFiles/mem_socket_client.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/projects/cloudCodingDB/mem_socket_client/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/mem_socket_client.dir/src/connector.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/mem_socket_client.dir/src/connector.cpp.o -MF CMakeFiles/mem_socket_client.dir/src/connector.cpp.o.d -o CMakeFiles/mem_socket_client.dir/src/connector.cpp.o -c /home/projects/cloudCodingDB/mem_socket_client/src/connector.cpp

CMakeFiles/mem_socket_client.dir/src/connector.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/mem_socket_client.dir/src/connector.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/projects/cloudCodingDB/mem_socket_client/src/connector.cpp > CMakeFiles/mem_socket_client.dir/src/connector.cpp.i

CMakeFiles/mem_socket_client.dir/src/connector.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/mem_socket_client.dir/src/connector.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/projects/cloudCodingDB/mem_socket_client/src/connector.cpp -o CMakeFiles/mem_socket_client.dir/src/connector.cpp.s

CMakeFiles/mem_socket_client.dir/src/inet_address.cpp.o: CMakeFiles/mem_socket_client.dir/flags.make
CMakeFiles/mem_socket_client.dir/src/inet_address.cpp.o: ../src/inet_address.cpp
CMakeFiles/mem_socket_client.dir/src/inet_address.cpp.o: CMakeFiles/mem_socket_client.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/projects/cloudCodingDB/mem_socket_client/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/mem_socket_client.dir/src/inet_address.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/mem_socket_client.dir/src/inet_address.cpp.o -MF CMakeFiles/mem_socket_client.dir/src/inet_address.cpp.o.d -o CMakeFiles/mem_socket_client.dir/src/inet_address.cpp.o -c /home/projects/cloudCodingDB/mem_socket_client/src/inet_address.cpp

CMakeFiles/mem_socket_client.dir/src/inet_address.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/mem_socket_client.dir/src/inet_address.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/projects/cloudCodingDB/mem_socket_client/src/inet_address.cpp > CMakeFiles/mem_socket_client.dir/src/inet_address.cpp.i

CMakeFiles/mem_socket_client.dir/src/inet_address.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/mem_socket_client.dir/src/inet_address.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/projects/cloudCodingDB/mem_socket_client/src/inet_address.cpp -o CMakeFiles/mem_socket_client.dir/src/inet_address.cpp.s

CMakeFiles/mem_socket_client.dir/src/socket.cpp.o: CMakeFiles/mem_socket_client.dir/flags.make
CMakeFiles/mem_socket_client.dir/src/socket.cpp.o: ../src/socket.cpp
CMakeFiles/mem_socket_client.dir/src/socket.cpp.o: CMakeFiles/mem_socket_client.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/projects/cloudCodingDB/mem_socket_client/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/mem_socket_client.dir/src/socket.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/mem_socket_client.dir/src/socket.cpp.o -MF CMakeFiles/mem_socket_client.dir/src/socket.cpp.o.d -o CMakeFiles/mem_socket_client.dir/src/socket.cpp.o -c /home/projects/cloudCodingDB/mem_socket_client/src/socket.cpp

CMakeFiles/mem_socket_client.dir/src/socket.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/mem_socket_client.dir/src/socket.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/projects/cloudCodingDB/mem_socket_client/src/socket.cpp > CMakeFiles/mem_socket_client.dir/src/socket.cpp.i

CMakeFiles/mem_socket_client.dir/src/socket.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/mem_socket_client.dir/src/socket.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/projects/cloudCodingDB/mem_socket_client/src/socket.cpp -o CMakeFiles/mem_socket_client.dir/src/socket.cpp.s

CMakeFiles/mem_socket_client.dir/src/util.cpp.o: CMakeFiles/mem_socket_client.dir/flags.make
CMakeFiles/mem_socket_client.dir/src/util.cpp.o: ../src/util.cpp
CMakeFiles/mem_socket_client.dir/src/util.cpp.o: CMakeFiles/mem_socket_client.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/projects/cloudCodingDB/mem_socket_client/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/mem_socket_client.dir/src/util.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/mem_socket_client.dir/src/util.cpp.o -MF CMakeFiles/mem_socket_client.dir/src/util.cpp.o.d -o CMakeFiles/mem_socket_client.dir/src/util.cpp.o -c /home/projects/cloudCodingDB/mem_socket_client/src/util.cpp

CMakeFiles/mem_socket_client.dir/src/util.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/mem_socket_client.dir/src/util.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/projects/cloudCodingDB/mem_socket_client/src/util.cpp > CMakeFiles/mem_socket_client.dir/src/util.cpp.i

CMakeFiles/mem_socket_client.dir/src/util.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/mem_socket_client.dir/src/util.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/projects/cloudCodingDB/mem_socket_client/src/util.cpp -o CMakeFiles/mem_socket_client.dir/src/util.cpp.s

# Object files for target mem_socket_client
mem_socket_client_OBJECTS = \
"CMakeFiles/mem_socket_client.dir/src/connector.cpp.o" \
"CMakeFiles/mem_socket_client.dir/src/inet_address.cpp.o" \
"CMakeFiles/mem_socket_client.dir/src/socket.cpp.o" \
"CMakeFiles/mem_socket_client.dir/src/util.cpp.o"

# External object files for target mem_socket_client
mem_socket_client_EXTERNAL_OBJECTS =

../lib/libmem_socket_client.so: CMakeFiles/mem_socket_client.dir/src/connector.cpp.o
../lib/libmem_socket_client.so: CMakeFiles/mem_socket_client.dir/src/inet_address.cpp.o
../lib/libmem_socket_client.so: CMakeFiles/mem_socket_client.dir/src/socket.cpp.o
../lib/libmem_socket_client.so: CMakeFiles/mem_socket_client.dir/src/util.cpp.o
../lib/libmem_socket_client.so: CMakeFiles/mem_socket_client.dir/build.make
../lib/libmem_socket_client.so: CMakeFiles/mem_socket_client.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/projects/cloudCodingDB/mem_socket_client/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX shared library ../lib/libmem_socket_client.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/mem_socket_client.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/mem_socket_client.dir/build: ../lib/libmem_socket_client.so
.PHONY : CMakeFiles/mem_socket_client.dir/build

CMakeFiles/mem_socket_client.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/mem_socket_client.dir/cmake_clean.cmake
.PHONY : CMakeFiles/mem_socket_client.dir/clean

CMakeFiles/mem_socket_client.dir/depend:
	cd /home/projects/cloudCodingDB/mem_socket_client/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/projects/cloudCodingDB/mem_socket_client /home/projects/cloudCodingDB/mem_socket_client /home/projects/cloudCodingDB/mem_socket_client/build /home/projects/cloudCodingDB/mem_socket_client/build /home/projects/cloudCodingDB/mem_socket_client/build/CMakeFiles/mem_socket_client.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/mem_socket_client.dir/depend

