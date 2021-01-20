#pragma once

#include <cstdint>
#include <cstdlib>
#include <memory>

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

    uint32_t vertex_count{};
    uint32_t index_count{};

    static inline size_t stride = sizeof (VertexData);

    static inline size_t vertex_offset = 0;
    static inline size_t joint_index_offset = sizeof(glm::vec3);
    static inline size_t joint_weight_offset = sizeof(glm::vec3) + sizeof(glm::vec4);

    static inline uint32_t vertex_layout = 0;
    static inline uint32_t joint_index_layout = 1;
    static inline uint32_t joint_weight_layout = 2;
};
