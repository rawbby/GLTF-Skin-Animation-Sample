#pragma once

#include <model/Types.hpp>

#include <cstdint>
#include <cstdlib>
#include <memory>

namespace model
{
    struct SkinnedMesh
    {
        struct VertexData
        {
            glm::vec3 vertex{};
            glm::vec4 joint_index{};
            glm::vec4 joint_weight{};
        };

        std::unique_ptr<VertexData[]> vertices{};
        std::unique_ptr<uint32_t[]> indices{};

        int32_t vertex_count{};
        int32_t index_count{};

        static inline int32_t stride = sizeof(VertexData);

        static inline const void *vertex_offset = nullptr;
        static inline const void *joint_index_offset = reinterpret_cast< const void *> (sizeof(glm::vec3));
        static inline const void *joint_weight_offset = reinterpret_cast< const void *> (sizeof(glm::vec3) + sizeof(glm::vec4));

        static inline uint32_t vertex_layout = 0;
        static inline uint32_t joint_index_layout = 1;
        static inline uint32_t joint_weight_layout = 2;
    };
}
