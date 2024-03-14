
project(harbor_manager)

set(CMAKE_CXX_STANDARD 17)

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
        ship.h
        bfs.cpp
        bfs.h
        queue.cpp
        queue.h
        map_object.cpp
        map_object.h
        map_area.cpp
        map_area.h
        route.cpp
        route.h
        deque.cpp
        deque.h
        list.cpp
        list.h
        thread_pool.h
)
