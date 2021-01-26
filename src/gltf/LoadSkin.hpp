#pragma once

#ifndef CGLTF_IMPLEMENTATION
#define CGLTF_IMPLEMENTATION
#include <cgltf.h>
#endif

#include <gltf/Types.hpp>
#include <gltf/Util.hpp>

#include <util/Assert.hpp>
#include <model/Skin.hpp>

#include <algorithm>

namespace gltf
{
    model::Skin load_single_skin (cgltf_data *data, internal::SkinExtra &skin_extra)
    {
        ASSERT(data->skins_count, "Can not load single Skin, if there is no skin at all!");

        skin_extra.skin_index = 0;
        auto &skin = data->skins[0];
        auto my_skin = model::Skin::prepare(skin.joints_count);

        ASSERT(skin.joints_count > 0, "Skin has not joints!");

        cgltf_node *skin_root;
        cgltf_node *scene_root;
        internal::skin_root_node(*data, skin, skin_root, scene_root);

        std::vector<std::pair<cgltf_node *, model::joint_index_t>> todo{};
        for (size_t i = 0; i < skin_root->children_count; ++i)
        {
            todo.emplace_back(skin_root->children[i], -1);
        }

        model::joint_index_t current_index = 0;
        while (!todo.empty())
        {
            auto[node, parent] = todo.back();
            todo.pop_back();

            auto joint_index = internal::joint_index(node, skin);
            if (joint_index == static_cast<size_t> (-1))
            {
                continue;
            }

            my_skin.joint_parent_indices[current_index] = parent;
            skin_extra.joint_map[joint_index] = static_cast<uint8_t> (current_index);

            for (size_t i = 0; i < node->children_count; ++i)
            {
                todo.emplace_back(node->children[i], current_index);
            }

            current_index++;
            ASSERT(current_index > 0, "Too many Joint loaded!");
        }

        ASSERT(current_index == skin.joints_count, "Failed to load all joints! Hierarchy invalid! {} != {}", current_index, skin.joints_count);

        // TODO check if it is needed to apply all parent transforms of skeleton root
        my_skin.joint_global_inverse = glm::inverse(internal::model_view_matrix(scene_root));

        auto *accessor = skin.inverse_bind_matrices;
        for (const auto&[i, data] : internal::Accessor<glm::mat4>{accessor})
        {
            my_skin.joint_bind_inverses[skin_extra.joint_map.at(i)] = *data;
        }

        return my_skin;
    }
}
