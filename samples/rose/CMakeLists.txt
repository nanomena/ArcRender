include_directories(../../include)
include_directories(../../deps)

set(name "sample_rose")

add_executable(${name} rose.cpp)

target_link_libraries(${name} arc)
target_link_libraries(${name} stb_image)
target_link_libraries(${name} obj_loader)