cmake_minimum_required(VERSION 3.16)
project(ARCrender)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)


if (CMAKE_BUILD_TYPE EQUAL Release)
    add_compile_options(-fopenmp)
    add_compile_options(-Ofast)
elseif (CMAKE_BUILD_TYPE EQUAL Debug)
    add_compile_options(-g)
    add_compile_options(-fsanitize=undefined)
    add_compile_options(-ftrapv)
endif ()


add_library(stb_image STATIC deps/stb_image.cpp deps/stb_image_write.cpp)
add_library(obj_loader STATIC deps/tiny_obj_loader.cpp)

include_directories(deps/)

add_library(arc STATIC include/arc.cpp)

find_package(OpenMP)
target_link_libraries(arc PUBLIC OpenMP::OpenMP_CXX)

add_subdirectory(samples)