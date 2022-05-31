cmake_minimum_required(VERSION 3.21)
project(program)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic")


add_executable(program image_processor.cpp filters.h filters_applier.h image.h image_parsers.h string_parser.h logger.h filters.cpp image_parser.cpp string_parser.cpp filters_applier.cpp logger.cpp)


target_link_libraries(program ${CMAKE_EXE_LINKER_FLAGS})