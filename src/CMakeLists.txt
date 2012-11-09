
cmake_minimum_required(VERSION 2.6)
 
project(mothertask)

 
file(
	GLOB_RECURSE 
	Source_File 
	${PROJECT_SOURCE_DIR}/*.cpp 
	${PROJECT_SOURCE_DIR}/*.h
    )
 
IF(UNIX)

find_package (Threads)
ENDIF(UNIX)
 
add_executable(FindMyPath ${Source_File})
 


