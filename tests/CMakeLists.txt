message(VERBOSE "Configuring ${CMAKE_CURRENT_LIST_FILE}")

set(TARGET tests)
add_executable(${TARGET})

target_sources(${TARGET} PRIVATE
        const_buffer.cpp
        main.cpp
        mutable_buffer.cpp
        )
