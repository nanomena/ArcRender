include_directories(../include)
include_directories(../deps)

file(GLOB SAMPLES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")
message(STATUS "targets: ${SAMPLES}")

foreach(SAMPLE IN LISTS SAMPLES)
    message(STATUS "target: ${SAMPLE}")
    string(REPLACE .cpp "" TARGET ${SAMPLE})
    add_executable(${TARGET} ${SAMPLE})

    target_link_libraries(${TARGET} arc)
    target_link_libraries(${TARGET} stb_image)
    target_link_libraries(${TARGET} obj_loader)
endforeach()