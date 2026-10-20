
add_executable(cara_example_minimal
    minimal/main.c
)

target_link_libraries(cara_example_minimal
    PRIVATE cara
)

enable_warnings(cara_example_minimal)
