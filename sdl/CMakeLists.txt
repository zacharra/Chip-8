set(SOURCES
    main.cpp
    window.cpp
    sdl.cpp
    ../external/tinyfiledialogs/tinyfiledialogs.c
)

set(HEADERS
    include/sdl.h
    include/window.h
    include/stopwatch.h
    ../external/tinyfiledialogs/tinyfiledialogs.h
)

find_package(SDL2 REQUIRED)

include_directories(
    ./include
    ../external/cxxopts/include
    ../external/tinyfiledialogs
    ${SDL2_INCLUDE_DIR}
)

set(LIBRARIES "${CMAKE_PROJECT_NAME}_lib" fmt ${FILESYSTEM_LIBRARIES} ${SDL2_LIBRARY})

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${CMAKE_PROJECT_NAME} ${LIBRARIES})

install(TARGETS ${CMAKE_PROJECT_NAME} EXPORT ${CMAKE_PROJECT_NAME} DESTINATION bin)
