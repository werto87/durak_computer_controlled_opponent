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

target_link_libraries(_test
        durak_computer_controlled_opponent
        Catch2::Catch2
        )

target_include_directories(_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR})

add_test(NAME _test
        COMMAND _test -r junit)