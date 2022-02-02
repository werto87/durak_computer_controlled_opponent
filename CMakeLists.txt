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
set(TARGET_NAME_LIB project_lib)
add_library(${TARGET_NAME_LIB} INTERFACE)
add_subdirectory(src) 
target_link_libraries(${TARGET_NAME_LIB} INTERFACE
${PROJECT_WARNINGS}
${PROJECT_OPTIONS}
  CONAN_PKG::boost
  CONAN_PKG::range-v3
)
target_compile_definitions(${TARGET_NAME_LIB} INTERFACE 
$<$<CONFIG:DEBUG>:DEBUG>
_LIBCPP_ENABLE_CXX20_REMOVED_TYPE_TRAITS
# BOOST_ASIO_ENABLE_HANDLER_TRACKING
# LOGGING_FOR_STATE_MACHINE
# LOGGING_CO_SPAWN_PRINT_EXCEPTIONS
)
# create executable
OPTION(BUILD_TESTS "enable tests" OFF)
IF(BUILD_TESTS)
    enable_testing()
    add_subdirectory(test) 
ENDIF(BUILD_TESTS)

             


# OPTION(BUILD_CUDA "enable tests" OFF)

# if(BUILD_CUDA)
# cmake_minimum_required(VERSION 3.20)
# project(project VERSION 1.0.0
#   DESCRIPTION project
#   LANGUAGES CUDA CXX
#   ) 
# else()
# cmake_minimum_required(VERSION 3.20)
# project(project VERSION 1.0.0
#   DESCRIPTION project
#   LANGUAGES CXX
#   ) 
# endif()





# cmake_policy(SET CMP0015 NEW)

# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

# if(CMAKE_BUILD_TYPE STREQUAL  "Debug")
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ")
#   set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ")
# else()
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")
#   set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Ofast")
# endif()


# ########## enable cache system
# include(cmake/Cache.cmake)

# ########## link against the libs to apply the configurations to the target
# # compiler warnings
# set(PROJECT_WARNINGS project_warning)
# add_library(${PROJECT_WARNINGS} INTERFACE)
# include(cmake/CompilerWarnings.cmake)
# set_project_warnings(${PROJECT_WARNINGS})
# # compiler options
# set(PROJECT_OPTIONS project_options)

# add_library(${PROJECT_OPTIONS} INTERFACE)
# target_compile_features(${PROJECT_OPTIONS} INTERFACE cxx_std_20)
# # target_precompile_headers(${PROJECT_OPTIONS} INTERFACE 
# # )


#  #include(cmake/Sanitizers.cmake)
#  #enable_sanitizers(${PROJECT_OPTIONS})

# # allow for static analysis options
# #include(cmake/StaticAnalyzers.cmake) 
 
# ######## conan setup
# include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
# conan_basic_setup(TARGETS)



# set(TARGET_NAME project)
# add_executable(${TARGET_NAME})


# if(BUILD_CUDA)
# find_package(CUDAToolkit)
# set_property(TARGET ${TARGET_NAME} PROPERTY CUDA_STANDARD 20)
# set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} --extended-lambda")
# endif()




# ######## add sub dir
# add_subdirectory(src) 


# ####### link
# target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
# target_include_directories(${TARGET_NAME} PRIVATE
#   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
#   $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
#   )
  



# if(BUILD_CUDA)
#   target_link_libraries(${TARGET_NAME} PRIVATE
#     ${PROJECT_WARNINGS}
#     ${PROJECT_OPTIONS}
#     CUDA::toolkit
#     CONAN_PKG::durak
#     CONAN_PKG::boost
#     CONAN_PKG::thrust
#     pthread
#   )
# else()
#   target_link_libraries(${TARGET_NAME} PRIVATE
#     ${PROJECT_WARNINGS}
#     ${PROJECT_OPTIONS}
#     CONAN_PKG::boost
#   )
# endif()


# ####### create executable
# OPTION(BUILD_TESTS "enable tests" OFF)
# IF(BUILD_TESTS)
#     enable_testing()
#     add_subdirectory(test) 
# ENDIF(BUILD_TESTS)

# # add main after  add_subdirectory(test) so we can take the sources from ${TARGET_NAME} and dont have to remove main.cxx from that list
# target_sources(${TARGET_NAME} PRIVATE 
# "main.cxx"
# )
 
# if (CMAKE_BINARY_DIR)
# target_compile_definitions(${TARGET_NAME} PRIVATE PATH_TO_BINARY="${CMAKE_BINARY_DIR}")
# endif()

# target_compile_definitions(${TARGET_NAME} PUBLIC "$<$<CONFIG:DEBUG>:DEBUG>")
# if(BUILD_CUDA)
# target_compile_definitions(${TARGET_NAME} PUBLIC BUILD_CUDA)
# endif()
# ####### include what you use
# #find_program(iwyu_path NAMES include-what-you-use iwyu)
# #set_property(TARGET ${TARGET_NAME} PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path})


# #print all variables used in cmake
# # get_cmake_property(_variableNames VARIABLES)
# # list (SORT _variableNames)
# # foreach (_variableName ${_variableNames})
# #    message(STATUS "${_variableName}=${${_variableName}}")
# # endforeach()

