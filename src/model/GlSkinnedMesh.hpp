#pragma once

#include <util/Assert.hpp>

#include <model/Types.hpp>
#include <model/SkinnedMesh.hpp>

#include <cstdint>

namespace model
{
    struct GlSkinnedMesh
    {
        gl_uint_t vao = 0;
        gl_size_t index_count = -1;

        static GlSkinnedMesh fromSkinnedMesh (const SkinnedMesh &mesh)
        {
            GlSkinnedMesh my_mesh{};
            my_mesh.index_count = mesh.index_count;

            glGenVertexArrays(1, &my_mesh.vao);
            glBindVertexArray(my_mesh.vao);
            ASSERT_OPEN_GL_STATUS();

            GLuint vertex_buffer = GL_NONE;
            glGenBuffers(1, &vertex_buffer);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
            glBufferData(GL_ARRAY_BUFFER, mesh.vertex_count * SkinnedMesh::stride, mesh.vertices.data(), GL_STATIC_DRAW);
            ASSERT_OPEN_GL_STATUS();

            glVertexAttribPointer(SkinnedMesh::vertex_layout, 3, GL_FLOAT, GL_FALSE, SkinnedMesh::stride, SkinnedMesh::vertex_offset);
            glEnableVertexAttribArray(SkinnedMesh::vertex_layout);
            ASSERT_OPEN_GL_STATUS();

            glVertexAttribPointer(SkinnedMesh::joint_index_layout, 4, GL_FLOAT, GL_FALSE, SkinnedMesh::stride, SkinnedMesh::joint_index_offset);
            glEnableVertexAttribArray(SkinnedMesh::joint_index_layout);
            ASSERT_OPEN_GL_STATUS();

            glVertexAttribPointer(SkinnedMesh::joint_weight_layout, 4, GL_FLOAT, GL_FALSE, SkinnedMesh::stride, SkinnedMesh::joint_weight_offset);
            glEnableVertexAttribArray(SkinnedMesh::joint_weight_layout);
            ASSERT_OPEN_GL_STATUS();

            GLuint index_buffer = GL_NONE;
            glGenBuffers(1, &index_buffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.index_count * sizeof(uint32_t), mesh.indices.data(), GL_STATIC_DRAW);
            ASSERT_OPEN_GL_STATUS();

            glBindVertexArray(GL_NONE);
            glDeleteBuffers(1, &vertex_buffer);
            glDeleteBuffers(1, &index_buffer);
            ASSERT_OPEN_GL_STATUS();

            return my_mesh;
        }
    };
}
