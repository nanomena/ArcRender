cmake_minimum_required(VERSION 3.16)
project(ARCrender)

set(CMAKE_CXX_STANDARD 17)


if (CMAKE_BUILD_TYPE EQUAL Debug)
    message("CMAKE IN DEBUG MODE")
elseif(CMAKE_BUILD_TYPE EQUAL Release)
    message("CMAKE IN RELEASE MODE")

    find_package(OpenMP)
    IF (OPENMP_FOUND)
        message("Found OpenMP!")
        add_compile_options(-fopenmp)
    ENDIF ()
    add_compile_options(-Ofast)
endif ()


add_library(stb_image STATIC 3rdparty/stb_image.cpp 3rdparty/stb_image_write.cpp)
add_library(obj_loader STATIC 3rdparty/tiny_obj_loader.cpp)

include_directories(3rdparty/)

add_library(arc STATIC include/arc.cpp)

add_subdirectory(samples)