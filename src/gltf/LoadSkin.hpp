#pragma once

#ifndef CGLTF_IMPLEMENTATION
#define CGLTF_IMPLEMENTATION
#include <cgltf.h>
#endif

#include <gltf/Types.hpp>
#include <gltf/Util.hpp>

#include <util/Assert.hpp>
#include <model/Skin.hpp>

namespace gltf
{
    void init_skin (cgltf_skin &skin, model::Skin &my_skin)
    {
        my_skin.joint_global_inverse = glm::identity<glm::mat4>();
        my_skin.joint_count = skin.joints_count;
        my_skin.joint_bind_inverses = std::make_unique<glm::mat4[]>(my_skin.joint_count);
        my_skin.joint_parent_indices = std::make_unique<model::joint_index_t[]>(my_skin.joint_count);
    }

    void load_skin (cgltf_skin &skin, cgltf_data &data, model::Skin &my_skin, internal::joint_map_t &joint_map)
    {
        init_skin(skin, my_skin);
        ASSERT(skin.joints_count > 0, "Skin has not joints!");

        cgltf_node *skin_root;
        cgltf_node *scene_root;
        internal::skin_root_node(data, skin, skin_root, scene_root);

        std::vector<std::pair<cgltf_node *, model::joint_index_t>> todo{};
        for (cgltf_size i = 0; i < skin_root->children_count; ++i)
        {
            todo.emplace_back(skin_root->children[i], -1);
        }

        model::joint_index_t current_index = 0;
        while (!todo.empty())
        {
            auto[node, parent] = todo.back();
            todo.pop_back();

            if (!internal::in_skin(node, skin))
            {
                continue;
            }

            my_skin.joint_parent_indices[current_index] = parent;
            joint_map[node] = static_cast<uint8_t> (current_index);

            for (cgltf_size i = 0; i < node->children_count; ++i)
            {
                todo.emplace_back(node->children[i], current_index);
            }

            current_index++;
            ASSERT(current_index > 0, "Too many Joint loaded!");
        }

        ASSERT(current_index == skin.joints_count, "Failed to load all joints! Hierarchy invalid! {} != {}", current_index, skin.joints_count);

        auto matrix = glm::identity<glm::mat4>();

        if (scene_root->has_matrix)
        {
            memcpy(&matrix, scene_root->matrix, sizeof(glm::mat4));
        }
        if (scene_root->has_translation)
        {
            glm::vec3 translation(scene_root->translation[0], scene_root->translation[1], scene_root->translation[2]);
            matrix = glm::translate(matrix, translation);
        }
        if (scene_root->has_rotation)
        {
            glm::quat rotation(scene_root->rotation[0], scene_root->rotation[1], scene_root->rotation[2], scene_root->rotation[3]);
            matrix = glm::toMat4(rotation) * matrix;
        }
        if (scene_root->has_scale)
        {
            glm::vec3 scale(scene_root->scale[0], scene_root->scale[1], scene_root->scale[2]);
            matrix = glm::scale(matrix, scale);
        }

        // TODO check if it is needed to apply all parent transforms of skeleton root
        my_skin.joint_global_inverse = glm::inverse(matrix);

        auto *accessor = skin.inverse_bind_matrices;
        auto offset = internal::accessor_offset(accessor);
        auto stride = internal::accessor_stride(accessor);

        auto *buffer_data = reinterpret_cast <char *>(accessor->buffer_view->buffer->data);

        for (cgltf_size i = 0; i < skin.joints_count; ++i)
        {
            auto *joint = skin.joints[i];
            auto *joint_bind_inverse = &(buffer_data[offset + i * stride]);

            my_skin.joint_bind_inverses[joint_map.at(joint)] = *(reinterpret_cast<glm::mat4 *> (joint_bind_inverse));
        }
    }
}
