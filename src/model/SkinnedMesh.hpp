#pragma once

#include <model/HeapArray.hpp>

#include <memory>
#include <cstdint>
#include <cstdlib>

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

        HeapArray <VertexData> vertices{};
        HeapArray <uint32_t> indices{};

        size_t vertex_count{};
        size_t index_count{};

        static SkinnedMesh prepare (size_t vertex_count, size_t index_count)
        {
            SkinnedMesh mesh;
            mesh.vertex_count = vertex_count;
            mesh.index_count = index_count;
            mesh.vertices = HeapArray<VertexData>(vertex_count);
            mesh.indices = HeapArray<uint32_t>(index_count);
            return mesh;
        }

        static constexpr gl_size_t stride = sizeof(VertexData);

        static constexpr gl_offset_t vertex_offset = nullptr;
        static inline auto joint_index_offset = reinterpret_cast<gl_offset_t> (sizeof(glm::vec3));
        static inline auto joint_weight_offset = reinterpret_cast<gl_offset_t> (sizeof(glm::vec3) + sizeof(glm::vec4));

        static constexpr gl_uint_t vertex_layout = 0;
        static constexpr gl_uint_t joint_index_layout = 1;
        static constexpr gl_uint_t joint_weight_layout = 2;
    };
}
