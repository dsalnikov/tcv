cmake_minimum_required(VERSION 2.8)
project(cv_app)
find_package(OpenCV REQUIRED)


set(SOURCE main.cpp) 
add_executable(cv_app ${SOURCE})	

add_subdirectory(tcv)
include_directories(tcv/inc)

target_link_libraries(cv_app tcv ${OpenCV_LIBS})