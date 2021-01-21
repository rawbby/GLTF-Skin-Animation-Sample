#pragma once

#ifndef CGLTF_IMPLEMENTATION
#define CGLTF_IMPLEMENTATION

#include <cgltf.h>

#endif

#include <model/Types.hpp>

namespace gltf::internal
{
    using joint_map = std::map<const cgltf_node *, model::size_t>;
}
