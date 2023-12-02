add_executable(2015
    src/main.c
    src/day01.c
    src/day02.c
    src/day03.c
    src/day04.c
    )


set(datadir ${CMAKE_CURRENT_SOURCE_DIR}/data)


add_custom_target(run2015 2015
    WORKING_DIRECTORY ${datadir}
    )


add_custom_target(debug2015
    gdb $<TARGET_FILE:2015>
    WORKING_DIRECTORY ${datadir}
    )
