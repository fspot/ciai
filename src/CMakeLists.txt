# CMakeLists du serveur
cmake_minimum_required(VERSION 2.6)
 
project(mothertask)
set (CMAKE_CXX_FLAGS "-std=c++0x") 
 
find_package (Threads)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
set(SOURCES sockets/network.cpp
	    sockets/parser.cpp
 	    sockets/netsend.cpp
	    mutex/mutex.cpp
	    log/log.cpp
	    remplirCarton/remplirCarton.cpp
	    controleur/controleur.cpp
	    destock/destock.cpp
	    stock/stock.cpp
	    remplirPalette/remplirPalette.cpp
	    stubs/stubs.cpp
	    imprimer/imprimer.cpp
	    piece/piece.cpp)

add_executable(mothertask mothertask.cpp ${SOURCES})
target_link_libraries (mothertask ${CMAKE_THREAD_LIBS_INIT})

