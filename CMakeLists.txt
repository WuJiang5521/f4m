cmake_minimum_required(VERSION 3.15)
project(LSH)

set(CMAKE_CXX_STANDARD 20)

add_executable(
        LSH
        test.cpp

        base/BaseAttr.cpp base/BaseAttr.hpp
        base/BaseSeq.cpp base/BaseSeq.hpp
        base/BaseEvt.cpp base/BaseEvt.hpp
        base/json.hpp
        base/FileLoader.cpp base/FileLoader.hpp

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
        DITTO/P_PTable.cpp
        DITTO/Window.cpp DITTO/Window.h base/BaseDS.cpp base/BaseDS.hpp)