cmake_minimum_required(VERSION 3.6)

set(CMAKE_C_COMPILER "gcc-9")
set(CMAKE_CXX_COMPILER "g++-9")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin/)
set(LIBRARY_OUTPUT_PATH ../lib)

project(ARCrender)

find_package(OpenMP REQUIRED)

include_directories(res/)

add_compile_options(-Ofast -fopenmp)
add_compile_options(-g)

add_library(stb_image STATIC res/stb_image.cpp)
add_library(obj_loader STATIC res/tiny_obj_loader.cpp)
add_library(arc STATIC src/arc.cpp)
link_directories(lib/)

include_directories(include/)

add_executable(surface samples/surface.cpp)
target_link_libraries(surface arc stb_image obj_loader OpenMP::OpenMP_CXX)

add_executable(texture samples/texture.cpp)
target_link_libraries(texture arc stb_image obj_loader OpenMP::OpenMP_CXX)

add_executable(model samples/model.cpp)
target_link_libraries(model arc stb_image obj_loader OpenMP::OpenMP_CXX)
