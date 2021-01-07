cmake_minimum_required(VERSION 3.15)
project(fmp)

set(CMAKE_CXX_STANDARD 20)

add_executable(fmp test.cpp Sequence/Sequence.cpp Sequence/Sequence.hpp json.hpp Pattern/Pattern.cpp Pattern/Pattern.hpp Fmp.cpp Fmp.hpp FileLoader.cpp FileLoader.hpp Event/Event.cpp Event/Event.hpp Attribute/Attribute.cpp Attribute/Attribute.hpp)