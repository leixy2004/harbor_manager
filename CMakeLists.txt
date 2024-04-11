cmake_minimum_required(VERSION 3.20)
project(harbor_manager)
set(CMAKE_CXX_STANDARD 17)
include_directories(.)
add_executable(harbor_manager
        robot.h
        main.cpp
        vector.cpp
        vector.h
        goods.h
        constant.h
        berth.h
        map.h
        position.h
        ship.cpp
        ship.h
        bfs.h
        map_object.h
        map_area.h
        terminal.h
        seller.h
)
add_executable(test
        test.cpp)