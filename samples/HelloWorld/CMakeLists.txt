include_directories(../../include)
include_directories(../../3rdparty)

set(name "HelloWorld")

add_executable(${name} Helloworld.cpp)

target_link_libraries(${name} arc)
target_link_libraries(${name} stb_image)
target_link_libraries(${name} obj_loader)