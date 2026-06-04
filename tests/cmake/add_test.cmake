# @file tests/cmake/add_test.cmake
# ===== Функция добавления тестов =====
function(add_unit_test TEST_NAME)
    add_executable(${TEST_NAME} ${ARGN})

    target_link_libraries(${TEST_NAME} PRIVATE
        ${CORE_NAME}
        GTest::gtest_main
    )

    set_target_properties(${TEST_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${TEST_BIN_DIR}
    )

    target_compile_options(${TEST_NAME} PRIVATE ${WARN_FLAGS})

    # Включить Coverage для тестов
    if (BUILD_COVERAGE)
        target_compile_options(${TEST_NAME} PRIVATE ${COVERAGE_COMPILE_FLAGS})
        target_link_options(${TEST_NAME} PRIVATE ${COVERAGE_LINK_FLAGS})
    endif()

    # add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    gtest_discover_tests(${TEST_NAME})

endfunction()
