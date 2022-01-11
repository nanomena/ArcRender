include_directories(../../include)
include_directories(../../deps)

set(name "caustics")

add_executable(${name} caustics.cpp)

target_link_libraries(${name} arc)
target_link_libraries(${name} stb_image)
target_link_libraries(${name} obj_loader)