# CMakeLists de test pour la tâche mère
cmake_minimum_required(VERSION 2.6)
 
project(testTacheImprimer)
#set (CMAKE_CXX_FLAGS "-fpermissive") 
 
find_package (Threads)
message(STATUS ${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/../../src/")
set(SOURCES ${PROJECT_SOURCE_DIR}/../../src/sockets/network.cpp
	    ${PROJECT_SOURCE_DIR}/../../src/sockets/parser.cpp
 	    ${PROJECT_SOURCE_DIR}/../../src/sockets/netsend.cpp
	    ${PROJECT_SOURCE_DIR}/../../src/multithreadObjects/mutex.cpp
	    ${PROJECT_SOURCE_DIR}/../../src/log/log.cpp
	    ${PROJECT_SOURCE_DIR}/../../src/remplirCarton/remplirCarton.cpp
	    ${PROJECT_SOURCE_DIR}/../../src/controleur/controleur.cpp
	    ${PROJECT_SOURCE_DIR}/../../src/gestionserie/gestionserie.cpp)

add_executable(testTacheImprimer testTacheImprimer.cpp ${SOURCES})
target_link_libraries (testTacheImprimer ${CMAKE_THREAD_LIBS_INIT})

