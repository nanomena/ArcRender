cmake_minimum_required(VERSION 3.6)

set(CMAKE_C_COMPILER "gcc-9")
set(CMAKE_CXX_COMPILER "g++-9")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin/)
set(LIBRARY_OUTPUT_PATH ../lib)

project(ARCrender)

find_package(OpenMP REQUIRED)
find_package(OpenCV REQUIRED)

add_compile_options(-Ofast -fopenmp)
add_library(arc STATIC src/arc.cpp)
link_directories(lib/)

include_directories(include/)
add_compile_options(-Dlibrary)
add_executable(surface samples/surface.cpp)
target_link_libraries(surface arc OpenMP::OpenMP_CXX)
