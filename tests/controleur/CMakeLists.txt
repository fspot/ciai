# CMakeLists de test pour la tâche controleur
cmake_minimum_required(VERSION 2.6)
 
project(testControleur)
#set (CMAKE_CXX_FLAGS "-fpermissive") 
 
find_package (Threads)
message(STATUS ${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/../../src/")
set( SOURCES_DIR ${PROJECT_SOURCE_DIR}/../../src/)
set(SOURCES ${SOURCES_DIR}/sockets/network.cpp
	    ${SOURCES_DIR}/sockets/parser.cpp
	    ${SOURCES_DIR}/multithreadObjects/mutex.cpp
	    ${SOURCES_DIR}/log/log.cpp
	    ${SOURCES_DIR}/controleur/controleur.cpp
	    ${SOURCES_DIR}/piece/piece.cpp)

add_executable(testControleur testControleur.cpp ${SOURCES})
target_link_libraries (testControleur ${CMAKE_THREAD_LIBS_INIT})

