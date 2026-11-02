add_library(cara STATIC
    src/cara.c
    src/event/src/cara_event_manager.c
    src/core/src/cara_core.c
)

if(WIN32)
    target_sources(cara PRIVATE
        src/platform/windows/cara_win32.c
        src/platform/windows/cara_win32_window.c
    )
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

target_include_directories(cara
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/internal/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src/event/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core/include
)
