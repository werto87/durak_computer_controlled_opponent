##
## Copyright (c) 2024 Waldemar Schneider (w-schneider1987 at web dot de)
##
## Distributed under the Boost Software License, Version 1.0.
## (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


add_library(durak_computer_controlled_opponent
        compressCard.cxx
        database.cxx
        permutation.cxx
        solve.cxx
        util.cxx
        )


add_library(myproject::durak_computer_controlled_opponent ALIAS durak_computer_controlled_opponent)


find_package(durak)
find_package(small_memory_tree)
find_package(st_tree)
find_package(stlplus)
find_package(confu_soci)
find_package(Boost)
find_package(magic_enum)
find_package(cereal)



target_link_libraries(durak_computer_controlled_opponent PUBLIC
        durak::durak small_memory_tree::small_memory_tree st_tree::st_tree stlplus::stlplus confu_soci::confu_soci boost::boost magic_enum::magic_enum cereal::cereal
        )

target_include_directories(durak_computer_controlled_opponent INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>)

target_compile_features(durak_computer_controlled_opponent PUBLIC cxx_std_20)

install(FILES
        combination.hxx
        compressCard.hxx
        database.hxx
        permutation.hxx
        solve.hxx
        util.hxx
        DESTINATION include/durak_computer_controlled_opponent
        )
install(TARGETS durak_computer_controlled_opponent DESTINATION lib)
