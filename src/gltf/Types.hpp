#pragma once

#ifndef CGLTF_IMPLEMENTATION
#define CGLTF_IMPLEMENTATION
#include <cgltf.h>
#endif

#include <model/Types.hpp>

namespace gltf::internal
{
    using joint_map_t = std::map<size_t, size_t>;
    using animation_map_t = std::map<std::string, size_t>;

    struct SkinExtra
    {
        joint_map_t joint_map{};
        size_t skin_index = 0;
    };

    struct AnimationsExtra
    {
        animation_map_t animation_map{};
        size_t animation_count = 0;
    };
}
