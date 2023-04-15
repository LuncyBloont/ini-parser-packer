cmake_minimum_required(VERSION 3.12.0)
project(inipp)

file(GLOB_RECURSE EXAMPLE_SRC "./example/*.cpp" "./example/*.h" "./example/*.hpp")
file(GLOB_RECURSE INIPP_SRC "./include/*.hpp" "./include/*.h")

add_executable(example ${EXAMPLE_SRC} ${INIPP_SRC} "./test0.ini")

target_include_directories(example PRIVATE "./include/")
