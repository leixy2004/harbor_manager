cmake_minimum_required(VERSION 3.27)
project(harbor_manager)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(harbor_manager
        robot.cpp
        robot.h
        main.cpp
        vector.cpp
        vector.h
        goods.cpp
        goods.h
        constant.h
        berth.cpp
        berth.h
        map.cpp
        map.h
        position.cpp
        position.h
        ship.cpp
        ship.h)
