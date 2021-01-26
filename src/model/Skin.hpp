#pragma once

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

        static Skin prepare (size_t joint_count)
        {
            Skin skin;
            skin.joint_bind_inverses = std::make_unique<glm::mat4[]>(joint_count);
            skin.joint_parent_indices = std::make_unique<joint_index_t[]>(joint_count);
            skin.joint_global_inverse = glm::identity<glm::mat4>();
            skin.joint_count = joint_count;
            return skin;
        }
    };
}
