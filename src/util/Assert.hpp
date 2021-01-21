#pragma once

#include <spdlog/spdlog.h>

#define ASSERT(CONDITION, MESSAGE, ...)   \
    if (!(CONDITION))                     \
    {                                     \
        spdlog::error("Assertion (file: {}, line: {}) \"{}\" failed! message: " MESSAGE, __FILE__, __LINE__, #CONDITION, __VA_ARGS__); \
        exit(-1);                         \
    }                                     \
    (void) 0

#define ASSERT_OPEN_GL_STATUS()           \
    {                                     \
        const auto status = glGetError(); \
        if (GL_NO_ERROR != status)        \
        {                                 \
            spdlog::error("Assertion (file: {}, line: {}) \"GL_NO_ERROR != glGetError()\" failed! status: {}", __FILE__, __LINE__, status); \
            exit(-1);                     \
        }                                 \
    }                                     \
    (void) 0
