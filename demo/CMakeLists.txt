cmake_minimum_required(VERSION 3.10.3)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 14)

add_definitions(
    -DMEDIA_STREAM
)

add_executable(ipc_demo
        src/main.cpp
        src/mongoose.c
        src/restApi.cpp
        src/acs_handlers.cpp
        src/acs_callback.cpp
        src/media_test.cpp
        )

include_directories(include)
include_directories(third_party/include)
include_directories(src)

target_link_libraries(ipc_demo
        ${CMAKE_SOURCE_DIR}/lib/liball_static.a

        dl
        pthread
        rt)

