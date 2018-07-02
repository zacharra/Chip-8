set(CHIP8_ROOT_PATH ${CMAKE_SOURCE_DIR})

set(CHIP8_SOURCES
    chip8.cpp
    utils/class_name.cpp)

set(CHIP8_HEADERS
    ../include/chip8/chip8.h
    ../include/chip8/exceptions.h
    ../include/chip8/utils/resource_ptr.h
    ../include/chip8/utils/random.h
    ../include/chip8/utils/class_name.h)

global_add_compiler_flags(-Wall -pedantic)

add_library(${CMAKE_PROJECT_NAME}_lib ${CHIP8_SOURCES} ${CHIP8_HEADERS})

target_include_directories(${CMAKE_PROJECT_NAME}_lib
  PUBLIC $<BUILD_INTERFACE:${CHIP8_ROOT_PATH}/include>
         $<INSTALL_INTERFACE:include>
  PRIVATE ${CHIP8_ROOT_PATH}/include/chip8
)

install(TARGETS ${CMAKE_PROJECT_NAME}_lib EXPORT ${CMAKE_PROJECT_NAME} DESTINATION lib)
install(FILES ${CHIP8_HEADERS} DESTINATION include/chip8)

