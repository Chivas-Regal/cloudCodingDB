cmake_minimum_required(VERSION 3.22.1)

project(mem_socket_server)

set(CMAKE_CXX_STANDARD 20)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

set(PROJ_PATH ${PROJECT_SOURCE_DIR})
configure_file(${PROJECT_SOURCE_DIR}/include/utils/proj_info.in.h ${PROJECT_SOURCE_DIR}/include/utils/proj_info.h)

# TEST =============================================================================
enable_testing()
add_executable(test_mempool test/mempool.cpp)
add_executable(test_sqlmanager test/sqlmanager.cpp)
add_executable(test_thread test/thread.cpp)
add_executable(test_sst_write_read test/sst_write_read.cpp)
target_link_libraries(test_mempool ${PROJECT_NAME} -lmysqlclient -lpthread)
target_link_libraries(test_sqlmanager ${PROJECT_NAME} -lmysqlclient -lpthread)
target_link_libraries(test_thread ${PROJECT_NAME} -lmysqlclient -lpthread)
target_link_libraries(test_sst_write_read ${PROJECT_NAME} -lmysqlclient -lpthread)

add_test(NAME test_mempool COMMAND test_mempool)
add_test(NAME test_sqlmanager COMMAND test_sqlmanager)
add_test(NAME test_thread COMMAND test_thread)
add_test(NAME test_sst_write_read COMMAND test_sst_write_read)
# ================================================================================== 

include_directories(
    ${PROJECT_SOURCE_DIR}/include/
    ${PROJECT_SOURCE_DIR}/include/memtools/
    ${PROJECT_SOURCE_DIR}/include/usrinfo/
    ${PROJECT_SOURCE_DIR}/include/threads/
    ${PROJECT_SOURCE_DIR}/include/containers/
    ${PROJECT_SOURCE_DIR}/include/sst/
    ${PROJECT_SOURCE_DIR}/include/containers/
    ${PROJECT_SOURCE_DIR}/include/cache/
    ${PROJECT_SOURCE_DIR}/include/db/
    ${PROJECT_SOURCE_DIR}/include/lsmtree/
    ${PROJECT_SOURCE_DIR}/include/wal/
    ${PROJECT_SOURCE_DIR}/include/utils/
    /usr/include/mysql/
)
aux_source_directory(./src SRC_FILES)
aux_source_directory(./src/memtools SRC_FILES)
aux_source_directory(./src/usrinfo SRC_FILES)
aux_source_directory(./src/threads SRC_FILES)
aux_source_directory(./src/containers SRC_FILES)
aux_source_directory(./src/sst SRC_FILES)
aux_source_directory(./src/db SRC_FILES)
aux_source_directory(./src/cache SRC_FILES)
aux_source_directory(./src/containers SRC_FILES)
aux_source_directory(./src/lsmtree SRC_FILES)
aux_source_directory(./src/wal SRC_FILES)
aux_source_directory(./src/utils SRC_FILES)

add_library(${PROJECT_NAME} SHARED ${SRC_FILES})

add_executable(memserverlib example/main.cpp)
target_link_libraries(memserverlib 
    ${PROJECT_NAME}
    -lmysqlclient
    -lpthread
)