if(CMAKE_COMPILER_IS_GNUCXX AND NOT (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.3) AND NOT MINGW)
    set(NEEDS_FILESYSTEM_LINKING 1)
else()
    set(NEEDS_FILESYSTEM_LINKING 0)
endif()

if(NEEDS_FILESYSTEM_LINKING)
    set(FILESYSTEM_LIBRARIES ${FILESYSTEM_LIBRARIES} -lstdc++fs)
endif()
