##
## Copyright (c) 2024 Waldemar Schneider (w-schneider1987 at web dot de)
##
## Distributed under the Boost Software License, Version 1.0.
## (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


add_library(durak_computer_controlled_opponent
        simulationLookup.cxx
        action.cxx
)


add_library(myproject::durak_computer_controlled_opponent ALIAS durak_computer_controlled_opponent)



find_package(durak)
find_package(small_memory_tree)
find_package(st_tree)
find_package(confu_soci)
find_package(Boost)
find_package(magic_enum)
find_package(cereal)



target_link_libraries(durak_computer_controlled_opponent PUBLIC
        durak::durak small_memory_tree::small_memory_tree st_tree::st_tree confu_soci::confu_soci Boost::headers magic_enum::magic_enum cereal::cereal
)

target_include_directories(durak_computer_controlled_opponent
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
)
target_compile_features(durak_computer_controlled_opponent PUBLIC cxx_std_20)

add_subdirectory(simulation)

install(FILES
        action.hxx
        util.hxx
        simulationLookup.hxx
        result.hxx
        DESTINATION include/durak_computer_controlled_opponent
)

install(TARGETS durak_computer_controlled_opponent DESTINATION lib)
