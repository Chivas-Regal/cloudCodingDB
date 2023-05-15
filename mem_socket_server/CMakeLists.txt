cmake_minimum_required(VERSION 3.22.1)

project(mem_socket_server)

set(CMAKE_CXX_STANDARD 20)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# TEST =============================================================================
enable_testing()
add_executable(test_mempool test/mempool.cpp)
add_executable(test_sqlmanager test/sqlmanager.cpp)
add_executable(test_thread test/thread.cpp)
target_link_libraries(test_mempool ${PROJECT_NAME} -lmysqlclient -lpthread)
target_link_libraries(test_sqlmanager ${PROJECT_NAME} -lmysqlclient -lpthread)
target_link_libraries(test_thread ${PROJECT_NAME} -lmysqlclient -lpthread)
add_test(NAME test_mempool COMMAND test_mempool)
add_test(NAME test_sqlmanager COMMAND test_sqlmanager)
add_test(NAME test_thread COMMAND test_thread)
# ================================================================================== 

include_directories(
    ${PROJECT_SOURCE_DIR}/include/
    ${PROJECT_SOURCE_DIR}/include/memtools/
    ${PROJECT_SOURCE_DIR}/include/usrinfo/
    ${PROJECT_SOURCE_DIR}/include/threads/
    /usr/include/mysql/
)
aux_source_directory(./src SRC_FILES)
aux_source_directory(./src/memtools SRC_FILES)
aux_source_directory(./src/usrinfo SRC_FILES)
aux_source_directory(./src/threads SRC_FILES)

add_library(${PROJECT_NAME} SHARED ${SRC_FILES})

add_executable(memserverlib example/main.cpp)
target_link_libraries(memserverlib 
    ${PROJECT_NAME}
    -lmysqlclient
    -lpthread
)