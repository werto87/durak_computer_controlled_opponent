include(CheckCXXCompilerFlag)

macro(
        myproject_enable_hardening
        target
)

    message(STATUS "** Enabling Hardening (Target ${target}) **")
    if (MSVC)
        set(NEW_COMPILE_OPTIONS "${NEW_COMPILE_OPTIONS} /sdl /DYNAMICBASE /guard:cf")
        message(STATUS "*** MSVC flags: /sdl /DYNAMICBASE /guard:cf /NXCOMPAT /CETCOMPAT")
        set(NEW_LINK_OPTIONS "${NEW_LINK_OPTIONS} /NXCOMPAT /CETCOMPAT")

    elseif (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang|GNU")
        set(NEW_CXX_DEFINITIONS "${NEW_CXX_DEFINITIONS} -D_GLIBCXX_ASSERTIONS")
        message(STATUS "*** GLIBC++ Assertions (vector[], string[], ...) enabled")
        if ()
            set(NEW_COMPILE_OPTIONS "${NEW_COMPILE_OPTIONS} -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3 -O") #_FORTIFY_SOURCE needs -O and not -g
            message(STATUS "*** g++/clang _FORTIFY_SOURCE=3 enabled")
        endif ()

        check_cxx_compiler_flag(-fstack-protector-strong STACK_PROTECTOR)
        if (STACK_PROTECTOR)
            set(NEW_COMPILE_OPTIONS "${NEW_COMPILE_OPTIONS} -fstack-protector-strong")
            message(STATUS "*** g++/clang -fstack-protector-strong enabled")
        else ()
            message(STATUS "*** g++/clang -fstack-protector-strong NOT enabled (not supported)")
        endif ()

        check_cxx_compiler_flag(-fcf-protection CF_PROTECTION)
        if (CF_PROTECTION)
            set(NEW_COMPILE_OPTIONS "${NEW_COMPILE_OPTIONS} -fcf-protection")
            message(STATUS "*** g++/clang -fcf-protection enabled")
        else ()
            message(STATUS "*** g++/clang -fcf-protection NOT enabled (not supported)")
        endif ()

        check_cxx_compiler_flag(-fstack-clash-protection CLASH_PROTECTION)
        if (CLASH_PROTECTION)
            if (LINUX OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
                set(NEW_COMPILE_OPTIONS "${NEW_COMPILE_OPTIONS} -fstack-clash-protection")
                message(STATUS "*** g++/clang -fstack-clash-protection enabled")
            else ()
                message(STATUS "*** g++/clang -fstack-clash-protection NOT enabled (clang on non-Linux)")
            endif ()
        else ()
            message(STATUS "*** g++/clang -fstack-clash-protection NOT enabled (not supported)")
        endif ()

    endif ()

    message(STATUS "** Hardening Compiler Flags: ${NEW_COMPILE_OPTIONS} ")
    message(STATUS "** Hardening Linker Flags: ${NEW_LINK_OPTIONS}")
    message(STATUS "** Hardening Compiler Defines: ${NEW_CXX_DEFINITIONS}")
endmacro()
