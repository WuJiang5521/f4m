cmake_minimum_required(VERSION 3.15)
project(fmp)

set(CMAKE_CXX_STANDARD 20)

add_executable(
        fmp
        test.cpp
        FMP/Sequence/FMPSequence.cpp FMP/Sequence/FMPSequence.hpp
        base/json.hpp
        FMP/Pattern/FmpPattern.cpp FMP/Pattern/FmpPattern.hpp
        FMP/Fmp.cpp FMP/Fmp.hpp
        base/FileLoader.cpp base/FileLoader.hpp
        FMP/Event/FMPEvent.cpp FMP/Event/FMPEvent.hpp
        FMP/Attribute/FMPAttribute.cpp FMP/Attribute/FMPAttribute.hpp
        FMP/MathUtil.h FMP/MathUtil.cpp
        FMP/P_PTable/P_PTable.cpp

        DITTO/CodeTable.cpp DITTO/CodeTable.h
        DITTO/cover.cpp DITTO/cover.h
        DITTO/defines.h
        DITTO/Ditto.cpp DITTO/Ditto.h
        DITTO/Event.h
        DITTO/getopt.cpp DITTO/getopt.h
        DITTO/mathutil.cpp DITTO/mathutil.h
        DITTO/Multi_event.cpp DITTO/Multi_event.h
        DITTO/Node.cpp DITTO/Node.h
        DITTO/params.h
        DITTO/DittoPattern.cpp DITTO/DittoPattern.h
        DITTO/DittoSequence.cpp DITTO/DittoSequence.h
        DITTO/stdafx.h
        DITTO/targetver.h
        DITTO/Window.cpp DITTO/Window.h
)