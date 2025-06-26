add_executable(_test
        compressCard.cxx
        database.cxx
        gameTree.cxx
        main.cxx
        permutation.cxx
        solve.cxx
        util.cxx
        )
find_package(Catch2)
find_package(fmt)
target_link_libraries(_test
        myproject_options
        myproject_warnings
        durak_computer_controlled_opponent
        Catch2::Catch2WithMain
        fmt::fmt
        )
if (CMAKE_BINARY_DIR)
    target_compile_definitions(_test PRIVATE CURRENT_BINARY_DIR="${CMAKE_BINARY_DIR}")
endif ()
target_include_directories(_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR})
add_test(NAME _test
        COMMAND _test -r junit)