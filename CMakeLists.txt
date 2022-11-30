# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Og")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Og")

cmake_minimum_required(VERSION 3.22)
project(project VERSION 1.0.0
        DESCRIPTION project
        LANGUAGES CXX
        )
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)
include(cmake/Cache.cmake)
set(PROJECT_WARNINGS project_warning)
add_library(${PROJECT_WARNINGS} INTERFACE)
include(cmake/CompilerWarnings.cmake)
set_project_warnings(${PROJECT_WARNINGS})
set(PROJECT_OPTIONS project_options)
add_library(${PROJECT_OPTIONS} INTERFACE)
target_compile_features(${PROJECT_OPTIONS} INTERFACE cxx_std_20)
include(cmake/StaticAnalyzers.cmake)
include(cmake/Sanitizers.cmake)
enable_sanitizers(${PROJECT_OPTIONS})
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(TARGET_NAME_LIB durak_computer_controlled_opponent)
add_library(${TARGET_NAME_LIB})
add_subdirectory(durak_computer_controlled_opponent)
target_link_libraries(${TARGET_NAME_LIB} PUBLIC
        ${PROJECT_WARNINGS}
        ${PROJECT_OPTIONS}
        CONAN_PKG::boost
        CONAN_PKG::range-v3
        CONAN_PKG::durak
        CONAN_PKG::st_tree
        CONAN_PKG::small_memory_tree
        CONAN_PKG::confu_soci
        )
target_compile_definitions(${TARGET_NAME_LIB} PUBLIC
        $<$<CONFIG:DEBUG>:DEBUG>
        _LIBCPP_ENABLE_CXX20_REMOVED_TYPE_TRAITS

        # BOOST_ASIO_ENABLE_HANDLER_TRACKING
        # LOGGING_FOR_STATE_MACHINE
        # LOGGING_CO_SPAWN_PRINT_EXCEPTIONS
        # RUN_BENCHMARK_SOLVE_DURAK
        # RUN_BENCHMARK_SOLVE
        )

# create executable
option(TEST "add test to the targets" OFF)
if (TEST)
    enable_testing()
    add_subdirectory(test)
endif ()
