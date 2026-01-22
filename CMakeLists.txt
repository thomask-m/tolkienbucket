cmake_minimum_required(VERSION 3.12)

project(Bucket VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(validate validate.cpp)
target_compile_options(validate PRIVATE -fsanitize=thread -Wall -Weffc++ -Wextra -Wsign-conversion)
target_link_options(validate PRIVATE -fsanitize=thread)
