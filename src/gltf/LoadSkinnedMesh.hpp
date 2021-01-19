#pragma once

#ifndef CGLTF_IMPLEMENTATION
#define CGLTF_IMPLEMENTATION
#include <cgltf.h>
#endif

#include <util/Assert.hpp>
#include <components/SkinnedMesh.hpp>

#include <cstdint>

namespace gltf
{
    void init_mesh (SkinnedMesh &my_mesh, int32_t vertex_count)
    {
        my_mesh.index_count = vertex_count;
    }

    void load_mesh (const cgltf_mesh &mesh, SkinnedMesh &my_mesh)
    {
        ASSERT(mesh.primitives_count == 1, "This loader only supports meshes with one triangle pack!");

        auto &primitive = mesh.primitives[0];

        ASSERT(primitive.indices->type == cgltf_type_scalar, "Index information invalid for mesh!");
        ASSERT(primitive.type == cgltf_primitive_type_triangles, "This loader only supports triangle meshes!");

        // ASSERT(primitive.attributes_count == 5, "This loader only supports meshes with the 5 attributes: vertices, normals, tex_coords, joint_indices and joint_weights!");

        init_mesh(my_mesh, primitive.indices->count);

        std::map<cgltf_buffer *, GLuint> gpu_buffers{};
        glGenVertexArrays(1, &my_mesh.vao);
        glBindVertexArray(my_mesh.vao);
        ASSERT_OPEN_GL_STATUS();

        bool vertices_loaded = false;
        bool normals_loaded = false;
        bool tex_coords_loaded = false;
        bool joint_indices_loaded = false;
        bool joint_weights_loaded = false;

        for (cgltf_size k = 0; k < primitive.attributes_count; ++k)
        {
            auto &attribute = primitive.attributes[k];
            auto *accessor = attribute.data;

            // continue if not supported:
            //@formatter:off
            if (attribute.type == cgltf_attribute_type_color) continue;
            if (attribute.type == cgltf_attribute_type_tangent) continue;
            if (attribute.type == cgltf_attribute_type_invalid) continue;
            //@formatter:on

            auto *buffer = attribute.data->buffer_view->buffer;
            if (!gpu_buffers.contains(buffer))
            {
                GLuint gpu_buffer = GL_NONE;
                glGenBuffers(1, &gpu_buffer);
                glBindBuffer(GL_ARRAY_BUFFER, gpu_buffer);
                glBufferData(GL_ARRAY_BUFFER, buffer->size, buffer->data, GL_STATIC_DRAW);
                ASSERT_OPEN_GL_STATUS();
                gpu_buffers[buffer] = gpu_buffer;
            }
            else
            {
                glBindBuffer(GL_ARRAY_BUFFER, gpu_buffers[buffer]);
            }

            const auto stride = static_cast<GLsizei> (accessor->stride);
            const auto *offset = reinterpret_cast <void *>(accessor->offset + accessor->buffer_view->offset);

            switch (attribute.type)
            {
                case cgltf_attribute_type_position:
                {
                    ASSERT(!(vertices_loaded), "Vertices already loaded!");
                    ASSERT(accessor->type == cgltf_type_vec3, "Vertex information invalid for mesh!");
                    ASSERT(accessor->component_type == cgltf_component_type_r_32f, "Vertex index information invalid for mesh!");
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, offset);
                    glEnableVertexAttribArray(0);
                    ASSERT_OPEN_GL_STATUS();
                    vertices_loaded = true;
                    break;
                }
                case cgltf_attribute_type_normal:
                {
                    ASSERT(!(normals_loaded), "Normals already loaded!");
                    ASSERT(accessor->type == cgltf_type_vec3, "Normal information invalid for mesh!");
                    ASSERT(accessor->component_type == cgltf_component_type_r_32f, "Normal index information invalid for mesh!");
                    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, offset);
                    glEnableVertexAttribArray(1);
                    ASSERT_OPEN_GL_STATUS();
                    normals_loaded = true;
                    break;
                }
                case cgltf_attribute_type_texcoord:
                {
                    ASSERT(!(tex_coords_loaded), "Tex Coords already loaded!");
                    ASSERT(accessor->type == cgltf_type_vec2, "Texture coordinate information invalid for mesh!");
                    ASSERT(accessor->component_type == cgltf_component_type_r_32f, "Texture coordinate index information invalid for mesh!");
                    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, offset);
                    glEnableVertexAttribArray(2);
                    ASSERT_OPEN_GL_STATUS();
                    tex_coords_loaded = true;
                    break;
                }
                case cgltf_attribute_type_joints:
                {
                    ASSERT(!(joint_indices_loaded), "Joint indices already loaded!");
                    ASSERT(accessor->type == cgltf_type_vec4, "Joint index information invalid for mesh!");
                    ASSERT(accessor->component_type == cgltf_component_type_r_8u, "Joint index information invalid for mesh!");
                    glVertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_FALSE, stride, offset);
                    glEnableVertexAttribArray(3);
                    ASSERT_OPEN_GL_STATUS();
                    joint_indices_loaded = true;
                    break;
                }
                case cgltf_attribute_type_weights:
                {
                    ASSERT(!(joint_weights_loaded), "Joint weights already loaded!");
                    ASSERT(accessor->type == cgltf_type_vec4, "Joint weight information invalid for mesh!");
                    ASSERT(accessor->component_type == cgltf_component_type_r_32f, "Joint weight information invalid for mesh!");
                    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, stride, offset);
                    glEnableVertexAttribArray(4);
                    ASSERT_OPEN_GL_STATUS();
                    joint_weights_loaded = true;
                    break;
                }
                default:
                ASSERT(0, "This loader only supports the following meshes attributes: vertices, normals, tex_coords, joint_indices and joint_weights!");
            }
        }

        ASSERT(vertices_loaded, "No Vertices to load!");
        ASSERT(normals_loaded, "No Normals to load!");
        ASSERT(tex_coords_loaded, "No Tex Coords to load!");
        ASSERT(joint_indices_loaded, "No Joint Indices to load!");
        ASSERT(joint_weights_loaded, "No Joint Weights to load!");

        auto *index_buffer_view = primitive.indices->buffer_view;
        auto *index_buffer = index_buffer_view->buffer;
        auto *index_buffer_data = &(reinterpret_cast<char *>(index_buffer->data)[index_buffer_view->offset]);
        const auto index_buffer_size = static_cast<GLsizei> (index_buffer_view->size);

        GLuint gpu_index_buffer = GL_NONE;
        glGenBuffers(1, &gpu_index_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpu_index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_size, index_buffer_data, GL_STATIC_DRAW);
        ASSERT_OPEN_GL_STATUS();

        glBindVertexArray(GL_NONE);
        for (const auto &gpu_buffer: gpu_buffers)
        {
            glDeleteBuffers(1, &gpu_buffer.second);
        }
        glDeleteBuffers(1, &gpu_index_buffer);
        ASSERT_OPEN_GL_STATUS();
    }
}
