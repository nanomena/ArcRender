include_directories(../../include)
include_directories(../../deps)

set(name "scene8")

add_executable(${name} scene8.cpp)

target_link_libraries(${name} arc)
target_link_libraries(${name} stb_image)
target_link_libraries(${name} obj_loader)