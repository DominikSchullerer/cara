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


# Examples
option(CARA_BUILD_EXAMPLES "Build CARA examples" ON)

if(CARA_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

enable_warnings(cara)