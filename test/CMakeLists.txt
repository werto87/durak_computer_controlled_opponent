set(TEST_TARGET_NAME _test)
add_executable(${TEST_TARGET_NAME} 
main.cxx
test.cxx
permutation.cxx
constant.hxx
)

target_link_libraries(${TEST_TARGET_NAME}
    ${TARGET_NAME_LIB}
    CONAN_PKG::catch2
)
add_test(NAME ${TEST_TARGET_NAME}
    COMMAND ${TEST_TARGET_NAME} -r junit
) 

target_compile_definitions(${TARGET_NAME_LIB} INTERFACE PATH_TO_USER_DEFINED_GAME_OPTION="test/userDefinedGameOption.hxx")

