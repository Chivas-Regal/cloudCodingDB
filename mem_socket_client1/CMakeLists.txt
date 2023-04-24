cmake_minimum_required(VERSION 3.22.1)

project(mem_socket_client)

set(CMAKE_CXX_STANDARD 20)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
add_compile_options(-pthread)

include_directories(
        ${PROJECT_SOURCE_DIR}/include/
)

aux_source_directory(./src SRC_FILES)
add_library(${PROJECT_NAME} SHARED ${SRC_FILES} include/socket.h include/inet_address.h src/socket.cpp src/inet_address.cpp include/limits.h)

add_executable(memclientlib example/main.cpp)
target_link_libraries(memclientlib
        ${OpenCV_LIBS}
        ${PROJECT_NAME}
)