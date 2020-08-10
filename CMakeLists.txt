cmake_minimum_required(VERSION 3.16)
project(ARCrender)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenMP)
IF (OPENMP_FOUND)
    message("Found OpenMP!")
    add_compile_options(-fopenmp)
ENDIF ()
add_compile_options(-Ofast)

add_library(stb_image STATIC 3rdparty/stb_image.cpp)
add_library(obj_loader STATIC 3rdparty/tiny_obj_loader.cpp)

include_directories(3rdparty/)

add_library(arc STATIC include/arc.cpp)

add_subdirectory(samples)