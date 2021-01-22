#pragma once

#ifndef CGLTF_IMPLEMENTATION
#define CGLTF_IMPLEMENTATION
#include <cgltf.h>
#endif

#include <gltf/Types.hpp>
#include <gltf/Util.hpp>

#include <util/Assert.hpp>
#include <model/SkinnedMesh.hpp>

#include <cstdint>

namespace gltf
{
    namespace internal
    {
        void load_vertices (model::SkinnedMesh &my_mesh, cgltf_primitive &primitive)
        {
            auto *attribute = attribute_by_type(primitive, cgltf_attribute_type_position);
            auto *accessor = attribute->data;

            ASSERT(accessor->count == my_mesh.vertex_count, "Invalid vertex count for attribute {}!", attribute->name);
            ASSERT(accessor->component_type == cgltf_component_type_r_32f, "Invalid component type in accessor of attribute {}!", attribute->name);
            ASSERT(accessor->type == cgltf_type_vec3, "Invalid component count in accessor of attribute {}!", attribute->name);

            for (const auto &[i, data] : internal::Accessor<glm::vec3>{accessor})
            {
                my_mesh.vertices[i].vertex = *data;
            }
        }

        void load_joint_indices (model::SkinnedMesh &my_mesh, cgltf_primitive &primitive, cgltf_skin &skin, internal::joint_map_t &joint_map)
        {
            auto *attribute = attribute_by_type(primitive, cgltf_attribute_type_joints);
            auto *accessor = attribute->data;

            ASSERT(accessor->count == my_mesh.vertex_count, "Invalid vertex count for attribute {}!", attribute->name);
            ASSERT(accessor->type == cgltf_type_vec4, "Invalid component count in accessor of attribute {}!", attribute->name);

            switch (accessor->component_type)
            {
                case cgltf_component_type_r_8:

                    for (const auto &[i, data] : internal::Accessor<int8_t[4]>{accessor})
                    {
                        my_mesh.vertices[i].joint_index[0] = static_cast<float> ((*data)[0]);
                        my_mesh.vertices[i].joint_index[1] = static_cast<float> ((*data)[1]);
                        my_mesh.vertices[i].joint_index[2] = static_cast<float> ((*data)[2]);
                        my_mesh.vertices[i].joint_index[3] = static_cast<float> ((*data)[3]);
                    }
                    break;

                case cgltf_component_type_r_8u:

                    for (const auto &[i, data] : internal::Accessor<uint8_t[4]>{accessor})
                    {
                        my_mesh.vertices[i].joint_index[0] = static_cast<float> ((*data)[0]);
                        my_mesh.vertices[i].joint_index[1] = static_cast<float> ((*data)[1]);
                        my_mesh.vertices[i].joint_index[2] = static_cast<float> ((*data)[2]);
                        my_mesh.vertices[i].joint_index[3] = static_cast<float> ((*data)[3]);
                    }
                    break;

                case cgltf_component_type_r_16:

                    for (const auto &[i, data] : internal::Accessor<int16_t[4]>{accessor})
                    {
                        my_mesh.vertices[i].joint_index[0] = static_cast<float> ((*data)[0]);
                        my_mesh.vertices[i].joint_index[1] = static_cast<float> ((*data)[1]);
                        my_mesh.vertices[i].joint_index[2] = static_cast<float> ((*data)[2]);
                        my_mesh.vertices[i].joint_index[3] = static_cast<float> ((*data)[3]);
                    }
                    break;

                case cgltf_component_type_r_16u:

                    for (const auto &[i, data] : internal::Accessor<uint16_t[4]>{accessor})
                    {
                        my_mesh.vertices[i].joint_index[0] = static_cast<float> ((*data)[0]);
                        my_mesh.vertices[i].joint_index[1] = static_cast<float> ((*data)[1]);
                        my_mesh.vertices[i].joint_index[2] = static_cast<float> ((*data)[2]);
                        my_mesh.vertices[i].joint_index[3] = static_cast<float> ((*data)[3]);
                    }
                    break;

                case cgltf_component_type_r_32u:

                    for (const auto &[i, data] : internal::Accessor<uint32_t[4]>{accessor})
                    {
                        my_mesh.vertices[i].joint_index[0] = static_cast<float> ((*data)[0]);
                        my_mesh.vertices[i].joint_index[1] = static_cast<float> ((*data)[1]);
                        my_mesh.vertices[i].joint_index[2] = static_cast<float> ((*data)[2]);
                        my_mesh.vertices[i].joint_index[3] = static_cast<float> ((*data)[3]);
                    }
                    break;

                case cgltf_component_type_r_32f:

                    for (const auto &[i, data] : internal::Accessor<float[4]>{accessor})
                    {
                        my_mesh.vertices[i].joint_index[0] = (*data)[0];
                        my_mesh.vertices[i].joint_index[1] = (*data)[1];
                        my_mesh.vertices[i].joint_index[2] = (*data)[2];
                        my_mesh.vertices[i].joint_index[3] = (*data)[3];
                    }
                    break;

                default:
                ASSERT(0, "Asked for component_size of invalid component_type!");
            }

            for (size_t i = 0; i < my_mesh.vertex_count; ++i)
            {
                auto index_x = static_cast<size_t> (my_mesh.vertices[i].joint_index.x);
                auto index_y = static_cast<size_t> (my_mesh.vertices[i].joint_index.y);
                auto index_z = static_cast<size_t> (my_mesh.vertices[i].joint_index.z);
                auto index_w = static_cast<size_t> (my_mesh.vertices[i].joint_index.w);

                my_mesh.vertices[i].joint_index.x = static_cast<float> (joint_map.at(skin.joints[index_x]));
                my_mesh.vertices[i].joint_index.y = static_cast<float> (joint_map.at(skin.joints[index_y]));
                my_mesh.vertices[i].joint_index.z = static_cast<float> (joint_map.at(skin.joints[index_z]));
                my_mesh.vertices[i].joint_index.w = static_cast<float> (joint_map.at(skin.joints[index_w]));
            }
        }

        void load_joint_weights (model::SkinnedMesh &my_mesh, cgltf_primitive &primitive)
        {
            auto *attribute = attribute_by_type(primitive, cgltf_attribute_type_weights);
            auto *accessor = attribute->data;

            ASSERT(accessor->count == my_mesh.vertex_count, "Invalid vertex count for attribute {}!", attribute->name);
            ASSERT(accessor->component_type == cgltf_component_type_r_32f, "Invalid component type in accessor of attribute {}!", attribute->name);
            ASSERT(accessor->type == cgltf_type_vec4, "Invalid component count in accessor of attribute {}!", attribute->name);

            for (const auto &[i, data] : internal::Accessor<glm::vec4>{accessor})
            {
                my_mesh.vertices[i].joint_weight = *data;
            }
        }

        void load_indices (model::SkinnedMesh &my_mesh, cgltf_primitive &primitive)
        {
            auto *accessor = primitive.indices;

            ASSERT(accessor->count == my_mesh.index_count, "Invalid m_index count for primitive!");
            ASSERT(accessor->type == cgltf_type_scalar, "Invalid component count in accessor of primitive indices!");

            switch (accessor->component_type)
            {
                case cgltf_component_type_r_8:

                    for (const auto &[i, data] : internal::Accessor<int8_t>{accessor})
                    {
                        my_mesh.indices[i] = static_cast<uint32_t> (static_cast<uint16_t> (*data));
                    }
                    break;

                case cgltf_component_type_r_8u:

                    for (const auto &[i, data] : internal::Accessor<uint8_t>{accessor})
                    {
                        my_mesh.indices[i] = static_cast<uint32_t> (*data);
                    }
                    break;

                case cgltf_component_type_r_16:

                    for (const auto &[i, data] : internal::Accessor<int16_t>{accessor})
                    {
                        my_mesh.indices[i] = static_cast<uint32_t> (*data);
                    }
                    break;

                case cgltf_component_type_r_16u:

                    for (const auto &[i, data] : internal::Accessor<uint16_t>{accessor})
                    {
                        my_mesh.indices[i] = static_cast<uint32_t> (*data);
                    }
                    break;

                case cgltf_component_type_r_32u:

                    for (const auto &[i, data] : internal::Accessor<uint32_t>{accessor})
                    {
                        my_mesh.indices[i] = *data;
                    }
                    break;

                case cgltf_component_type_r_32f:

                    for (const auto &[i, data] : internal::Accessor<float>{accessor})
                    {
                        my_mesh.indices[i] = static_cast<uint32_t> (*data);
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

    void load_mesh (cgltf_mesh &mesh, model::SkinnedMesh &my_mesh, cgltf_skin &skin, internal::joint_map_t &joint_map)
    {
        ASSERT(mesh.primitives_count == 1, "This loader only supports meshes with one triangle pack!");

        auto &primitive = mesh.primitives[0];

        ASSERT(primitive.indices->type == cgltf_type_scalar, "Index information invalid for mesh!");
        ASSERT(primitive.type == cgltf_primitive_type_triangles, "This loader only supports triangle meshes!");

        init_mesh(my_mesh, primitive);

        internal::load_vertices(my_mesh, primitive);
        internal::load_joint_indices(my_mesh, primitive, skin, joint_map);
        internal::load_joint_weights(my_mesh, primitive);
        internal::load_indices(my_mesh, primitive);
    }
}
