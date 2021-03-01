cmake_minimum_required(VERSION 3.15)
project(BEEP)

set(CMAKE_CXX_STANDARD 20)

add_executable(
        BEEP
        test.cpp


        base/BaseAttr.cpp base/BaseAttr.hpp
        base/BaseSeq.cpp base/BaseSeq.hpp
        base/BaseEvt.cpp base/BaseEvt.hpp
        base/json.hpp
        base/FileLoader.cpp base/FileLoader.hpp

        BEEP/CodeTable.cpp BEEP/CodeTable.h
        BEEP/Cover.cpp BEEP/Cover.h
        BEEP/Defines.h
        BEEP/Beep.cpp BEEP/Beep.h
        BEEP/Attribute.h
        BEEP/GetOpt.cpp BEEP/GetOpt.h
        BEEP/MathUtil.cpp BEEP/MathUtil.h
        BEEP/Event.cpp BEEP/Event.h
        BEEP/Node.cpp BEEP/Node.h
        BEEP/Params.h
        BEEP/Pattern.cpp BEEP/Pattern.h
        BEEP/Sequence.cpp BEEP/Sequence.h
        BEEP/Common.h
        BEEP/PatternTable.cpp
        BEEP/Window.cpp BEEP/Window.h  base/BaseDS.cpp base/BaseDS.hpp)