include_directories(../../include)
include_directories(../../3rdparty)

set(name "test_texture")

add_executable(${name} texture.cpp)


IF (OPENMP_FOUND)
    target_link_libraries(${name} OpenMP::OpenMP_CXX)
ENDIF ()

target_link_libraries(${name} arc)
target_link_libraries(${name} stb_image)
target_link_libraries(${name} obj_loader)