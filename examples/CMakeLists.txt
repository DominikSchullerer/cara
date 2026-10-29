
add_executable(cara_example_minimal
    minimal/main.c
)

target_link_libraries(cara_example_minimal
    PRIVATE cara
)

enable_warnings(cara_example_minimal)


add_executable(cara_example_window
    window/main.c
)

target_link_libraries(cara_example_window
    PRIVATE cara
)

enable_warnings(cara_example_window)


add_executable(cara_example_events
    events/main.c
)

target_link_libraries(cara_example_events
    PRIVATE cara
)

enable_warnings(cara_example_events)
