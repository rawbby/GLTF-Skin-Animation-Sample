#pragma once

#include <model/Types.hpp>

#include <cstdint>

namespace model
{
    struct GlSkinnedMesh
    {
        gl_uint_t vao = 0;
        gl_size_t index_count = -1;
    };
}
