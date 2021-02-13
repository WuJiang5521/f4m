cmake_minimum_required(VERSION 3.15)
project(BEEP)

set(CMAKE_CXX_STANDARD 20)

add_executable(
        BEEP
        test.cpp

        BEEP/CodeTable.cpp BEEP/CodeTable.h
        BEEP/cover.cpp BEEP/cover.h
        BEEP/defines.h
        BEEP/Beep.cpp BEEP/Beep.h
        BEEP/Event.h
        BEEP/getopt.cpp BEEP/getopt.h
        BEEP/mathutil.cpp BEEP/mathutil.h
        BEEP/Multi_event.cpp BEEP/Multi_event.h
        BEEP/Node.cpp BEEP/Node.h
        BEEP/params.h
        BEEP/Pattern.cpp BEEP/Pattern.h
        BEEP/Sequence.cpp BEEP/Sequence.h
        BEEP/stdafx.h
        BEEP/targetver.h
        BEEP/P_P_Table.cpp
        BEEP/Window.cpp BEEP/Window.h)