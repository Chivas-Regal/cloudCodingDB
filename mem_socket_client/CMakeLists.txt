cmake_minimum_required(VERSION 3.22.1)

project(mem_socket_client)

set(CMAKE_CXX_STANDARD 20)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
add_compile_options(-pthread)

include_directories(${PROJECT_SOURCE_DIR}/include/)
aux_source_directory(./src SRC_FILES)
add_library(${PROJECT_NAME} SHARED ${SRC_FILES})


file(
    GLOB_RECURSE mains RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/localwork.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/example/memclientlib.cpp"
)
foreach(mainfile IN LISTS mains)
    get_filename_component(mainname ${mainfile} NAME_WE)
    add_executable(${mainname} ${mainfile})
    target_link_libraries(${mainname} ${PROJECT_NAME})
endforeach()