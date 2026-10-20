cmake_minimum_required(VERSION 3.20)

project(cara
    VERSION 1.0
    LANGUAGES C
)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

function(enable_warnings target)
    if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE
            -Wall
            -Wextra
            -Werror
        )
    elseif (MSVC)
        target_compile_options(${target} PRIVATE
            /W4
            /WX
        )
    endif()
endfunction()

add_subdirectory(cara)

# Manuelle Tests / Experimente
add_executable(cara_manual
    tests/manual/main.c
)

target_link_libraries(cara_manual
    PRIVATE
        cara
        cara_internal
)

# Automatische Tests
add_executable(cara_tests
    tests/automatic/main.c
)

target_link_libraries(cara_tests
    PRIVATE
        cara
        cara_internal
)

# Examples
option(CARA_BUILD_EXAMPLES "Build CARA examples" ON)

if(CARA_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

enable_warnings(cara)
enable_warnings(cara_manual)
enable_warnings(cara_tests)