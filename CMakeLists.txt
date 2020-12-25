cmake_minimum_required(VERSION 3.17)
project(fmp)

set(CMAKE_CXX_STANDARD 14)

add_executable(fmp test.cpp Sequence.cpp Sequence.hpp json.hpp Pattern.cpp Pattern.hpp Fmp.cpp Fmp.hpp FileLoader.cpp FileLoader.hpp Event.cpp Event.hpp Attribute.cpp Attribute.hpp)