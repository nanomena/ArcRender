include_directories(../../include)
include_directories(../../3rdparty)

set(name "scene9")

add_executable(${name} scene9.cpp)

target_link_libraries(${name} arc)
target_link_libraries(${name} stb_image)
target_link_libraries(${name} obj_loader)