set(TEST_TARGET_NAME _test)
add_executable(${TEST_TARGET_NAME}
    main.cxx
    permutation.cxx
    compressCard.cxx
    solve.cxx
    gameTree.cxx
        util.cxx)

target_link_libraries(${TEST_TARGET_NAME}
    ${TARGET_NAME_LIB}
    CONAN_PKG::catch2
)
add_test(NAME ${TEST_TARGET_NAME}
    COMMAND ${TEST_TARGET_NAME} -r junit
)

target_compile_definitions(${TEST_TARGET_NAME} PRIVATE

    # RUN_BENCHMARK
    # RUN_BENCHMARK_SOLVE_DURAK
)
