#pragma once

#include <glm/matrix.hpp>

#include <memory>
#include <cstdint>

struct Skin
{
    std::unique_ptr<glm::mat4[]> joint_bind_inverse{};
    std::unique_ptr<uint8_t[]> joint_parent_indices{};

    glm::mat4 joint_global_inverse{};
    uint8_t joint_count = 0;
};
