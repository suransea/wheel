link_libraries(
        wheel
)

aux_source_directory(. TEST_SRC)

add_executable(test ${TEST_SRC})
