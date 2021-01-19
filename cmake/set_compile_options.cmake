cmake_minimum_required(VERSION 3.18)

unset(CMAKE_CXX_FLAGS_DEBUG)
unset(CMAKE_CXX_FLAGS_RELEASE)
unset(CMAKE_CXX_FLAGS_RELWITHDEBINFO)
unset(CMAKE_CXX_FLAGS_MINSIZEREL)

function(set_compile_options THIS)

    set_target_properties(${THIS} PROPERTIES
            LINKER_LANGUAGE CXX
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED ON)

    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

        target_compile_options(${THIS} PRIVATE
                $<$<CONFIG:DEBUG>:-g -O0 -DDEBUG>
                $<$<CONFIG:RELEASE>:-O3 -DNDEBUG>
                $<$<CONFIG:RELWITHDEBINFO>:-g -O2 -DNDEBUG>
                $<$<CONFIG:MINSIZEREL>:-Os -DNDEBUG>)

    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

        target_compile_options(${THIS} PRIVATE
                $<$<CONFIG:DEBUG>:/Zi /Od /Ob0 /RTC1>
                $<$<CONFIG:RELEASE>:/O2 /Ob2 /DNDEBUG>
                $<$<CONFIG:RELWITHDEBINFO>:/Zi /O2 /Ob1 /DNDEBUG>
                $<$<CONFIG:MINSIZEREL>:/O1 /Ob1 /DNDEBUG>)

    else ()

        message(WARNING
                "No Options specified for ${CMAKE_CXX_COMPILER_ID}. "
                "Consider using one of the following compilers: Clang, GNU, MSVC.")

    endif ()

endfunction()
