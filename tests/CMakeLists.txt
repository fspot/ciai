# CMakeLists d'inclusion recursif des tests
cmake_minimum_required(VERSION 2.6)
set (CMAKE_CXX_FLAGS "-std=c++0x") 

add_subdirectory(controleur)
add_subdirectory(imprimer)
add_subdirectory(remplirPalette)
add_subdirectory(remplirCarton)
add_subdirectory(destockage)
add_subdirectory(stockage)

