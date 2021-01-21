#pragma once

#ifndef CGLTF_IMPLEMENTATION
#define CGLTF_IMPLEMENTATION
#include <cgltf.h>
#endif

#include <gltf/Types.hpp>

#include <util/Assert.hpp>
#include <model/SkinnedMesh.hpp>

#include <cstdint>

namespace gltf
{
    namespace internal
    {
        const cgltf_attribute *attribute_by_type (const cgltf_primitive &primitive, cgltf_attribute_type type, int index = 0)
        {
            for (cgltf_size i = 0; i < primitive.attributes_count; ++i)
            {
                auto &attribute = primitive.attributes[i];

                if (attribute.type == type && attribute.index == index)
                {
                    return &attribute;
                }
            }

            ASSERT(0, "Mesh does not provide attribute {} with index {}!", type, index);
        }

        cgltf_size component_size (const cgltf_accessor *accessor)
        {
            switch (accessor->component_type)
            {
                case cgltf_component_type_r_8:
                case cgltf_component_type_r_8u:
                    return 1;

                case cgltf_component_type_r_16:
                case cgltf_component_type_r_16u:
                    return 2;

                case cgltf_component_type_r_32u:
                case cgltf_component_type_r_32f:
                    return 4;

                case cgltf_component_type_invalid:
                ASSERT(0, "Asked for component_size of invalid component_type!");
                default:
                ASSERT(0, "Asked for component_size of unknown component_type!");
            }
        }

        cgltf_size component_count (const cgltf_accessor *accessor)
        {
            switch (accessor->type)
            {
                case cgltf_type_scalar:
                    return 1;
                case cgltf_type_vec2:
                    return 2;
                case cgltf_type_vec3:
                    return 3;
                case cgltf_type_vec4:
                case cgltf_type_mat2:
                    return 4;
                case cgltf_type_mat3:
                    return 9;
                case cgltf_type_mat4:
                    return 16;
                default:
                ASSERT(0, "Asked for component_count of invalid component_type!");
            }
        }

        cgltf_size accessor_offset (const cgltf_accessor *accessor)
        {
            return accessor->offset + accessor->buffer_view->offset;
        }

        cgltf_size accessor_stride (const cgltf_accessor *accessor)
        {
            return accessor->stride ? accessor->stride : component_size(accessor) * component_count(accessor);
        }

        void load_vertices (model::SkinnedMesh &my_mesh, const cgltf_primitive &primitive)
        {
            const auto *attribute = attribute_by_type(primitive, cgltf_attribute_type_position);
            const auto *accessor = attribute->data;
            const auto *data = reinterpret_cast<char *> (accessor->buffer_view->buffer->data);

            ASSERT(accessor->count == my_mesh.vertex_count, "Invalid vertex count for attribute {}!", attribute->name);
            ASSERT(accessor->component_type == cgltf_component_type_r_32f, "Invalid component type in accessor of attribute {}!", attribute->name);
            ASSERT(accessor->type == cgltf_type_vec3, "Invalid component count in accessor of attribute {}!", attribute->name);

            const auto offset = accessor_offset(accessor);
            const auto stride = accessor_stride(accessor);

            for (cgltf_size i = 0; i < accessor->count; ++i)
            {
                my_mesh.vertices[i].vertex = *(reinterpret_cast<const glm::vec3 *> (&(data[offset + i * stride])));
            }
        }

        void load_joint_indices (model::SkinnedMesh &my_mesh, const cgltf_primitive &primitive, const cgltf_skin &skin, std::map<std::string, int8_t> &joint_map)
        {
            const auto *attribute = attribute_by_type(primitive, cgltf_attribute_type_joints);
            const auto *accessor = attribute->data;
            const auto *buffer_view = accessor->buffer_view;
            const auto *buffer = buffer_view->buffer;
            const auto *data = reinterpret_cast<const char *> (buffer->data);

            ASSERT(accessor->count == my_mesh.vertex_count, "Invalid vertex count for attribute {}!", attribute->name);
            ASSERT(accessor->type == cgltf_type_vec4, "Invalid component count in accessor of attribute {}!", attribute->name);

            const auto offset = accessor_offset(accessor);
            const auto stride = accessor_stride(accessor);

            switch (accessor->component_type)
            {
                case cgltf_component_type_r_8:

                    for (cgltf_size i = 0; i < accessor->count; ++i)
                    {
                        const auto *joint_index = reinterpret_cast<const int8_t *> (&(data[offset + i * stride]));
                        my_mesh.vertices[i].joint_index[0] = static_cast<float> (joint_index[0]);
                        my_mesh.vertices[i].joint_index[1] = static_cast<float> (joint_index[1]);
                        my_mesh.vertices[i].joint_index[2] = static_cast<float> (joint_index[2]);
                        my_mesh.vertices[i].joint_index[3] = static_cast<float> (joint_index[3]);
                    }
                    break;

                case cgltf_component_type_r_8u:

                    for (cgltf_size i = 0; i < accessor->count; ++i)
                    {
                        const auto *joint_index = reinterpret_cast<const uint8_t *> (&(data[offset + i * stride]));
                        my_mesh.vertices[i].joint_index[0] = static_cast<float> (joint_index[0]);
                        my_mesh.vertices[i].joint_index[1] = static_cast<float> (joint_index[1]);
                        my_mesh.vertices[i].joint_index[2] = static_cast<float> (joint_index[2]);
                        my_mesh.vertices[i].joint_index[3] = static_cast<float> (joint_index[3]);
                    }
                    break;

                case cgltf_component_type_r_16:

                    for (cgltf_size i = 0; i < accessor->count; ++i)
                    {
                        const auto *joint_index = reinterpret_cast<const int16_t *> (&(data[offset + i * stride]));
                        my_mesh.vertices[i].joint_index[0] = static_cast<float> (joint_index[0]);
                        my_mesh.vertices[i].joint_index[1] = static_cast<float> (joint_index[1]);
                        my_mesh.vertices[i].joint_index[2] = static_cast<float> (joint_index[2]);
                        my_mesh.vertices[i].joint_index[3] = static_cast<float> (joint_index[3]);
                    }
                    break;

                case cgltf_component_type_r_16u:

                    for (cgltf_size i = 0; i < accessor->count; ++i)
                    {
                        const auto *joint_index = reinterpret_cast<const uint16_t *> (&(data[offset + i * stride]));
                        my_mesh.vertices[i].joint_index[0] = static_cast<float> (joint_index[0]);
                        my_mesh.vertices[i].joint_index[1] = static_cast<float> (joint_index[1]);
                        my_mesh.vertices[i].joint_index[2] = static_cast<float> (joint_index[2]);
                        my_mesh.vertices[i].joint_index[3] = static_cast<float> (joint_index[3]);
                    }
                    break;

                case cgltf_component_type_r_32u:

                    for (cgltf_size i = 0; i < accessor->count; ++i)
                    {
                        const auto *joint_index = reinterpret_cast<const uint32_t *> (&(data[offset + i * stride]));
                        my_mesh.vertices[i].joint_index[0] = static_cast<float> (joint_index[0]);
                        my_mesh.vertices[i].joint_index[1] = static_cast<float> (joint_index[1]);
                        my_mesh.vertices[i].joint_index[2] = static_cast<float> (joint_index[2]);
                        my_mesh.vertices[i].joint_index[3] = static_cast<float> (joint_index[3]);
                    }
                    break;

                case cgltf_component_type_r_32f:

                    for (cgltf_size i = 0; i < accessor->count; ++i)
                    {
                        const auto *joint_index = reinterpret_cast<const float *> (&(data[offset + i * stride]));
                        my_mesh.vertices[i].joint_index[0] = joint_index[0];
                        my_mesh.vertices[i].joint_index[1] = joint_index[1];
                        my_mesh.vertices[i].joint_index[2] = joint_index[2];
                        my_mesh.vertices[i].joint_index[3] = joint_index[3];
                    }
                    break;

                default:
                ASSERT(0, "Asked for component_size of invalid component_type!");
            }

            for (cgltf_size i = 0; i < accessor->count; ++i)
            {
                const auto index_x = static_cast<cgltf_size> (my_mesh.vertices[i].joint_index.x);
                const auto index_y = static_cast<cgltf_size> (my_mesh.vertices[i].joint_index.y);
                const auto index_z = static_cast<cgltf_size> (my_mesh.vertices[i].joint_index.z);
                const auto index_w = static_cast<cgltf_size> (my_mesh.vertices[i].joint_index.w);

                auto my_index_x = joint_map.at(skin.joints[index_x]->name);
                auto my_index_y = joint_map.at(skin.joints[index_y]->name);
                auto my_index_z = joint_map.at(skin.joints[index_z]->name);
                auto my_index_w = joint_map.at(skin.joints[index_w]->name);

                my_mesh.vertices[i].joint_index.x = static_cast<float> (my_index_x);
                my_mesh.vertices[i].joint_index.y = static_cast<float> (my_index_y);
                my_mesh.vertices[i].joint_index.z = static_cast<float> (my_index_z);
                my_mesh.vertices[i].joint_index.w = static_cast<float> (my_index_w);
            }
        }

        void load_joint_weights (model::SkinnedMesh &my_mesh, const cgltf_primitive &primitive)
        {
            const auto *attribute = attribute_by_type(primitive, cgltf_attribute_type_weights);
            const auto *accessor = attribute->data;
            const auto *buffer_view = accessor->buffer_view;
            const auto *buffer = buffer_view->buffer;
            const auto *data = reinterpret_cast<const char *> (buffer->data);

            ASSERT(accessor->count == my_mesh.vertex_count, "Invalid vertex count for attribute {}!", attribute->name);
            ASSERT(accessor->component_type == cgltf_component_type_r_32f, "Invalid component type in accessor of attribute {}!", attribute->name);
            ASSERT(accessor->type == cgltf_type_vec4, "Invalid component count in accessor of attribute {}!", attribute->name);

            const auto offset = accessor_offset(accessor);
            const auto stride = accessor_stride(accessor);

            for (cgltf_size i = 0; i < accessor->count; ++i)
            {
                my_mesh.vertices[i].joint_weight = *(reinterpret_cast<const glm::vec4 *> (&(data[offset + i * stride])));
            }
        }

        void load_indices (model::SkinnedMesh &my_mesh, const cgltf_primitive &primitive)
        {
            const auto *accessor = primitive.indices;
            const auto *buffer_view = accessor->buffer_view;
            const auto *buffer = buffer_view->buffer;
            const auto *data = reinterpret_cast<const char *> (buffer->data);

            ASSERT(accessor->count == my_mesh.index_count, "Invalid index count for primitive!");
            ASSERT(accessor->type == cgltf_type_scalar, "Invalid component count in accessor of primitive indices!");

            const auto offset = accessor_offset(accessor);
            const auto stride = accessor_stride(accessor);

            switch (accessor->component_type)
            {
                case cgltf_component_type_r_8:

                    for (cgltf_size i = 0; i < accessor->count; ++i)
                    {
                        const auto *index = reinterpret_cast<const int8_t *> (&(data[offset + i * stride]));
                        my_mesh.indices[i] = static_cast<uint32_t> (static_cast<uint16_t> (*index));
                    }
                    break;

                case cgltf_component_type_r_8u:

                    for (cgltf_size i = 0; i < accessor->count; ++i)
                    {
                        const auto *index = reinterpret_cast<const uint8_t *> (&(data[offset + i * stride]));
                        my_mesh.indices[i] = static_cast<uint32_t> (*index);
                    }
                    break;

                case cgltf_component_type_r_16:

                    for (cgltf_size i = 0; i < accessor->count; ++i)
                    {
                        const auto *index = reinterpret_cast<const int16_t *> (&(data[offset + i * stride]));
                        my_mesh.indices[i] = static_cast<uint32_t> (*index);
                    }
                    break;

                case cgltf_component_type_r_16u:

                    for (cgltf_size i = 0; i < accessor->count; ++i)
                    {
                        const auto *index = reinterpret_cast<const uint16_t *> (&(data[offset + i * stride]));
                        my_mesh.indices[i] = static_cast<uint32_t> (*index);
                    }
                    break;

                case cgltf_component_type_r_32u:

                    for (cgltf_size i = 0; i < accessor->count; ++i)
                    {
                        const auto *index = reinterpret_cast<const uint32_t *> (&(data[offset + i * stride]));
                        my_mesh.indices[i] = static_cast<uint32_t> (*index);
                    }
                    break;

                case cgltf_component_type_r_32f:

                    for (cgltf_size i = 0; i < accessor->count; ++i)
                    {
                        const auto *index = reinterpret_cast<const float *> (&(data[offset + i * stride]));
                        my_mesh.indices[i] = static_cast<uint32_t> (*index);
                    }
                    break;

                default:
                ASSERT(0, "Asked for component_size of invalid component_type!");
            }
        }
    }

    void init_mesh (model::SkinnedMesh &my_mesh, cgltf_primitive &primitive)
    {
        ASSERT(primitive.attributes_count, "No vertices for mesh!");

        my_mesh.vertex_count = primitive.attributes[0].data->count;
        my_mesh.index_count = primitive.indices->count;

        my_mesh.vertices = std::make_unique<model::SkinnedMesh::VertexData[]>(my_mesh.vertex_count);
        my_mesh.indices = std::make_unique<uint32_t[]>(my_mesh.index_count);
    }

    void load_mesh (const cgltf_mesh &mesh, model::SkinnedMesh &my_mesh, const cgltf_skin &skin, std::map<std::string, int8_t> &joint_map)
    {
        using namespace internal;

        ASSERT(mesh.primitives_count == 1, "This loader only supports meshes with one triangle pack!");

        auto &primitive = mesh.primitives[0];

        ASSERT(primitive.indices->type == cgltf_type_scalar, "Index information invalid for mesh!");
        ASSERT(primitive.type == cgltf_primitive_type_triangles, "This loader only supports triangle meshes!");

        init_mesh(my_mesh, primitive);

        load_vertices(my_mesh, primitive);
        load_joint_indices(my_mesh, primitive, skin, joint_map);
        load_joint_weights(my_mesh, primitive);
        load_indices(my_mesh, primitive);
    }
}
