include(CMakeDependentOption)
include(CheckCXXCompilerFlag)


macro(myproject_setup_options)
    option(myproject_ENABLE_HARDENING "Enable hardening" ON)
    option(myproject_ENABLE_FORTIFY_SOURCE_AND_SET_OPTIMAZATION_TO_O0 "Enable fortify source and set optimization to -O0" OFF)
    option(myproject_ENABLE_COVERAGE "Enable coverage reporting" OFF)
    option(myproject_WARNINGS_AS_ERRORS "Treat Warnings As Errors" ON)
    option(myproject_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(myproject_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(myproject_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(myproject_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(myproject_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(myproject_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(myproject_ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
    option(myproject_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(myproject_ENABLE_CACHE "Enable ccache" ON)
endmacro()


macro(myproject_local_options)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    add_library(myproject_warnings INTERFACE)
    add_library(myproject_options INTERFACE)
    include(cmake/CompilerWarnings.cmake)
    myproject_set_project_warnings(
            myproject_warnings
            ${myproject_WARNINGS_AS_ERRORS}
            ""
            ""
            ""
            "")
    if (myproject_ENABLE_USER_LINKER)
        include(cmake/Linker.cmake)
        configure_linker(myproject_options)
    endif ()

    include(cmake/Sanitizers.cmake)
    myproject_enable_sanitizers(
            myproject_options
            ${myproject_ENABLE_SANITIZER_ADDRESS}
            ${myproject_ENABLE_SANITIZER_LEAK}
            ${myproject_ENABLE_SANITIZER_UNDEFINED}
            ${myproject_ENABLE_SANITIZER_THREAD}
            ${myproject_ENABLE_SANITIZER_MEMORY})

    if (myproject_ENABLE_CACHE)
        include(cmake/Cache.cmake)
        myproject_enable_cache()
    endif ()

    include(cmake/StaticAnalyzers.cmake)
    if (myproject_ENABLE_CLANG_TIDY)
        myproject_enable_clang_tidy(myproject_options ${myproject_WARNINGS_AS_ERRORS})
    endif ()

    if (myproject_ENABLE_CPPCHECK)
        myproject_enable_cppcheck(${myproject_WARNINGS_AS_ERRORS} "" # override cppcheck options
                )
    endif ()

    if (myproject_ENABLE_COVERAGE)
        include(cmake/Tests.cmake)
        myproject_enable_coverage(myproject_options)
    endif ()

    if (myproject_WARNINGS_AS_ERRORS)
        check_cxx_compiler_flag("-Wl,--fatal-warnings" LINKER_FATAL_WARNINGS)
    endif ()

    if (myproject_ENABLE_HARDENING)
        include(cmake/Hardening.cmake)
        myproject_enable_hardening(myproject_options)
    endif ()

endmacro()
