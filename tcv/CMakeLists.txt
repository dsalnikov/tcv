cmake_minimum_required(VERSION 2.8)
project(tcv)
find_package(OpenCV REQUIRED)


set(SOURCE 
    src/aswm.cpp
    src/tcv.cpp
    src/GetTimeMs64.cpp
    src/fix16.c
    src/fix16_sqrt.c
    src/neon.cpp
    src/neon-median-filter.cpp
    src/sdm.cpp
) 
include_directories(inc)

add_library(tcv STATIC ${SOURCE})
target_link_libraries(tcv ${OpenCV_LIBS})