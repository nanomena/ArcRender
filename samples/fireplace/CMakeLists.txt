include_directories(../../include)
include_directories(../../deps)

set(name "fireplace")

add_executable(${name} fireplace.cpp)

target_link_libraries(${name} arc)
target_link_libraries(${name} stb_image)
target_link_libraries(${name} obj_loader)