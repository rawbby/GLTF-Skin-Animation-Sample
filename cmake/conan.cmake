cmake_minimum_required(VERSION 3.18)

if (NOT EXISTS ${CMAKE_BINARY_DIR}/conan.cmake)
    file(DOWNLOAD
            https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake
            ${CMAKE_BINARY_DIR}/conan.cmake
            TLS_VERIFY ON)
endif ()

include(${CMAKE_BINARY_DIR}/conan.cmake)
