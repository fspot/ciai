# CMakeLists de test pour la tâche mère
cmake_minimum_required(VERSION 2.6)
set (CMAKE_CXX_FLAGS "-g -fpermissive") 

add_subdirectory(controleur)
add_subdirectory(imprimer)
add_subdirectory(remplirPalette)
add_subdirectory(remplirCarton)
