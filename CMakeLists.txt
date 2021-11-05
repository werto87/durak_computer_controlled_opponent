cmake_minimum_required(VERSION 3.20)
project(project VERSION 1.0.0
  DESCRIPTION project
  LANGUAGES CUDA CXX
  ) 
cmake_policy(SET CMP0015 NEW)
######## conan setup
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)
set(TARGET_NAME project)
add_executable(${TARGET_NAME}
main.cxx
)
find_package(CUDAToolkit)
set_property(TARGET ${TARGET_NAME} PROPERTY CUDA_STANDARD 20)
set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} --extended-lambda")
######## add sub dir
add_subdirectory(src) 
####### link
target_include_directories(${TARGET_NAME} PRIVATE
${CMAKE_CURRENT_BINARY_DIR}
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )
target_link_libraries(${TARGET_NAME} PRIVATE
    CUDA::toolkit
    CONAN_PKG::thrust
    pthread
)