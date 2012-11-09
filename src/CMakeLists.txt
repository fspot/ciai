# CMakeLists de test pour la tâche mère
cmake_minimum_required(VERSION 2.6)
 
project(mothertask)
set (CMAKE_CXX_FLAGS "-fpermissive") 
 
find_package (Threads)

add_executable(mothertask mothertask.cpp)
target_link_libraries (mothertask ${CMAKE_THREAD_LIBS_INIT})

