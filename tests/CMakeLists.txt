message(VERBOSE "Configuring ${CMAKE_CURRENT_LIST_FILE}")

set(TARGET tests)
add_executable(${TARGET})

target_sources(${TARGET} PRIVATE
        main.cpp
        )
