add_library(cara STATIC
    src/cara.c
    src/platform/cara_platform.c
 "src/platform/cara_platform_internal.h")

target_include_directories(cara
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_library(cara_internal INTERFACE)

target_include_directories(cara_internal
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)