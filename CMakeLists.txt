cmake_minimum_required(VERSION 3.16)
project(ARCrender)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)


if (CMAKE_BUILD_TYPE EQUAL Single)
    add_compile_options(-Ofast)
elseif (CMAKE_BUILD_TYPE EQUAL Release)
    find_package(OpenMP REQUIRED)
    add_compile_options(-fopenmp)
    add_compile_options(-Ofast)
elseif (CMAKE_BUILD_TYPE EQUAL Debug)
    add_compile_options(-g)
    add_compile_options(-fsanitize=address)
endif ()


add_library(stb_image STATIC deps/stb_image.cpp deps/stb_image_write.cpp)
add_library(obj_loader STATIC deps/tiny_obj_loader.cpp)

include_directories(deps/)

add_library(arc STATIC include/arc.cpp)

add_subdirectory(samples)