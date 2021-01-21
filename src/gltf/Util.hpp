#pragma once

#ifndef CGLTF_IMPLEMENTATION
#define CGLTF_IMPLEMENTATION
#include <cgltf.h>
#endif

#include <gltf/Types.hpp>

namespace gltf::internal
{
    bool in_skin (cgltf_node *&node, cgltf_skin &skin)
    {
        for (cgltf_size j = 0; j < skin.joints_count; ++j)
        {
            if (skin.joints[j] == node)
            {
                return true;
            }
        }
        return false;
    }

    void skin_root_node (cgltf_data &data, cgltf_skin &skin, cgltf_node *&skin_root, cgltf_node *&scene_root)
    {
        std::vector<cgltf_node *> todo{};

        for (cgltf_size i = 0; i < data.scenes_count; ++i)
        {
            auto &scene = data.scene[i];

            for (cgltf_size j = 0; j < scene.nodes_count; ++j)
            {
                todo.push_back(scene.nodes[j]);

                while (!todo.empty())
                {
                    auto *node = todo.back();
                    todo.pop_back();

                    for (cgltf_size k = 0; k < node->children_count; ++k)
                    {
                        auto *child = node->children[k];

                        if (in_skin(child, skin))
                        {
                            skin_root = node;
                            scene_root = scene.nodes[j];
                            return;
                        }

                        todo.emplace_back(child);
                    }
                }
            }
        }

        ASSERT(0, "Can not find the root node of skin {}!", skin.name);
    }

    cgltf_attribute *attribute_by_type (cgltf_primitive &primitive, cgltf_attribute_type type, int index = 0)
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

    cgltf_size component_size (cgltf_accessor *accessor)
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

    cgltf_size component_count (cgltf_accessor *accessor)
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

    cgltf_size accessor_offset (cgltf_accessor *accessor)
    {
        return accessor->offset + accessor->buffer_view->offset;
    }

    cgltf_size accessor_stride (cgltf_accessor *accessor)
    {
        return accessor->stride ? accessor->stride : component_size(accessor) * component_count(accessor);
    }
}
