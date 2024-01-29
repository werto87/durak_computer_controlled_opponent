##
## Copyright (c) 2024 Waldemar Schneider (w-schneider1987 at web dot de)
##
## Distributed under the Boost Software License, Version 1.0.
## (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
##

add_library(durak_computer_controlled_opponent)

target_sources(durak_computer_controlled_opponent PUBLIC
        util.cxx
        solve.cxx
        permutation.cxx
        database.cxx
        compressCard.cxx
)

install(FILES
        combination.hxx
        compressCard.hxx
        database.hxx
        permutation.hxx
        solve.hxx
        util.hxx
        DESTINATION include/durak_computer_controlled_opponent
        )

find_package(durak)
find_package(small_memory_tree)
find_package(st_tree)
find_package(range-v3)
find_package(confu_soci)
find_package(Boost)
find_package(magic_enum)


target_link_libraries(durak_computer_controlled_opponent PUBLIC
        ${PROJECT_WARNINGS}
        ${PROJECT_OPTIONS}
        boost::boost
        magic_enum::magic_enum
        range-v3::range-v3
        durak::durak
        st_tree::st_tree
        small_memory_tree::small_memory_tree
        confu_soci::confu_soci
        )

# state that anybody linking to us needs to include the current source dir
# to find MathFunctions.h, while we don't.
target_include_directories(durak_computer_controlled_opponent
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
        )
install(TARGETS durak_computer_controlled_opponent DESTINATION lib)
