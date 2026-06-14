# To build the programm from base directory :
# mkdir build && cd build
# cmake ..
# make
# ./moc

cmake_minimum_required(VERSION 3.10)

# Prevent in-source builds — forces the user to use a separate build dir
# Not necessarily needed (sometimes build in the project directory is OK)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR
        "In-source builds are not allowed.\n"
        "Please create a build directory and run cmake from there:\n"
        "  mkdir build && cd build && cmake ..")
endif()

project(MoC)


set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Just list all source files together in the executable target.
# CMake will compile each .c file and link them into one binary.
add_executable(moc
    src/main.c
    src/parse.c
    src/commqueue.c
    src/tileheap.c
    src/tileset.c
    src/stb_implem.c
    src/img_utils.c
    src/threads/read_thread.c
    src/threads/prod_thread.c
    src/threads/fit_thread.c
)

target_include_directories(moc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Add libraries (namely math for stb_image)
target_link_libraries(moc PRIVATE
  m       # -lm (math)
  pthread # -lpthread
)

# Add compiler options
target_compile_options(moc PRIVATE
  -Wall
  -Wextra
  -O3
)
