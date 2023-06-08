cmake_minimum_required(VERSION 3.22)
project(MyProject)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# Find the Threads package
find_package(Threads REQUIRED)

add_executable(main main.c render.c multivisit.c)

# Link against the pthread and math libraries
target_link_libraries(main PRIVATE Threads::Threads m)

