#pragma once

#include <model/Types.hpp>

#include <glm/matrix.hpp>

#include <memory>
#include <cstdint>

namespace model
{
    struct Skin
    {
        std::unique_ptr<glm::mat4[]> joint_bind_inverses{};
        std::unique_ptr<joint_index_t[]> joint_parent_indices{};

        glm::mat4 joint_global_inverse{};
        size_t joint_count = 0;
    };
}
