include_directories(../../include)
include_directories(../../deps)

set(name "test_surface")

add_executable(${name} surface.cpp)

target_link_libraries(${name} arc)
target_link_libraries(${name} stb_image)
target_link_libraries(${name} obj_loader)