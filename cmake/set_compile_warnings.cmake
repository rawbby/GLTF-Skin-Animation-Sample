cmake_minimum_required(VERSION 3.18)

function(set_compile_warnings THIS)

    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")

        target_compile_options(${THIS} PRIVATE
#                $<$<CONFIG:RELEASE>:-Werror>
                $<$<CONFIG:Debug>:-Wshadow>
                $<$<CONFIG:Debug>:-Wunused>
                -Wall -Wextra
                -Wnon-virtual-dtor
                -Wold-style-cast
                -Wcast-align
                -Woverloaded-virtual
                -Wpedantic
                -Wconversion
                -Wsign-conversion
                -Wdouble-promotion
                -Wformat=2)

    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

        target_compile_options(${THIS} PRIVATE
#                $<$<CONFIG:RELEASE>:-Werror>
                $<$<CONFIG:Debug>:-Wshadow>
                $<$<CONFIG:Debug>:-Wunused>
                -Wall -Wextra
                -Wnon-virtual-dtor
                -Wold-style-cast
                -Wcast-align
                -Woverloaded-virtual
                -Wpedantic
                -Wconversion
                -Wsign-conversion
                -Wdouble-promotion
                -Wformat=2)

    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

        target_compile_options(${THIS} PRIVATE
#                $<$<CONFIG:RELEASE>:/WX>
                /W4
                /w14242 /w14254 /w14263
                /w14265 /w14287 /we4289
                /w14296 /w14311 /w14545
                /w14546 /w14547 /w14549
                /w14555 /w14619 /w14640
                /w14826 /w14905 /w14906
                /w14928)

    else ()

        message(WARNING
                "No Warnings specified for ${CMAKE_CXX_COMPILER_ID}. "
                "Consider using one of the following compilers: Clang, GNU, MSVC.")

    endif ()

endfunction()
