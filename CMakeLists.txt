cmake_minimum_required(VERSION 3.15.7)
if (NOT DEFINED CMAKE_CXX_STANDARD) # Only set the cxx_standard if it is not set by someone else
    set(CMAKE_CXX_STANDARD 20)
endif ()
project(
        myproject
        VERSION 0.0.1
        LANGUAGES CXX C)
include(cmake/PreventInSourceBuilds.cmake)
include(ProjectOptions.cmake)
myproject_setup_options()
myproject_local_options()
target_compile_features(myproject_options INTERFACE cxx_std_${CMAKE_CXX_STANDARD})
add_library(myproject::myproject_options ALIAS myproject_options)
add_library(myproject::myproject_warnings ALIAS myproject_warnings)
add_subdirectory(small_memory_tree)
OPTION(BUILD_TESTS "enable tests" OFF)
IF (BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
ENDIF (BUILD_TESTS)