#CMakeLists de test pour la tâche remplirCarton
cmake_minimum_required(VERSION 2.6)

project(testRemplirCarton)

find_package(Threads)
message(STATUS ${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/../../src/")
set(SOURCES ${PROJECT_SOURCE_DIR}/../../src/remplirCarton/remplirCarton.cpp
			${PROJECT_SOURCE_DIR}/../../src/remplirCarton/remplirCarton.h
			${PROJECT_SOURCE_DIR}/../../src/modeles/modeles.h
			${PROJECT_SOURCE_DIR}/../../src/mailbox/mailbox.h
			${PROJECT_SOURCE_DIR}/../../src/multithreadObjects/mutex.h
			${PROJECT_SOURCE_DIR}/../../src/multithreadObjects/mutex.cpp
			${PROJECT_SOURCE_DIR}/../../src/log/log.h
			${PROJECT_SOURCE_DIR}/../../src/log/log.cpp)

add_executable(testRemplirCarton testRemplirCarton.cpp ${SOURCES})
target_link_libraries(testRemplirCarton ${CMAKE_THREAD_LIBS_INIT})