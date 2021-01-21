#pragma once

#ifndef CGLTF_IMPLEMENTATION
#define CGLTF_IMPLEMENTATION
#include <cgltf.h>
#endif

#include <gltf/Types.hpp>

#include <util/Assert.hpp>
#include <model/Skin.hpp>

namespace gltf
{
    namespace internal
    {
        bool in_skin (const cgltf_node *&node, const cgltf_skin &skin)
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

        void skin_root_node (const cgltf_data &data, const cgltf_skin &skin, const cgltf_node *&skin_root, const cgltf_node *&scene_root)
        {
            std::vector<const cgltf_node *> todo{};

            for (cgltf_size i = 0; i < data.scenes_count; ++i)
            {
                const auto &scene = data.scene[i];

                for (cgltf_size j = 0; j < scene.nodes_count; ++j)
                {
                    todo.push_back(scene.nodes[j]);

                    while (!todo.empty())
                    {
                        const auto *node = todo.back();
                        todo.pop_back();

                        for (cgltf_size k = 0; k < node->children_count; ++k)
                        {
                            const auto *child = node->children[k];

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
    }

    void init_skin (const cgltf_skin &skin, model::Skin &my_skin)
    {
        my_skin.joint_global_inverse = glm::identity<glm::mat4>();
        my_skin.joint_count = static_cast<int8_t>(skin.joints_count);
        my_skin.joint_bind_inverse = std::make_unique<glm::mat4[]>(my_skin.joint_count);
        my_skin.joint_parent_indices = std::make_unique<int8_t[]>(my_skin.joint_count);
    }

    void load_skin (const cgltf_skin &skin, const cgltf_data &data, model::Skin &my_skin, std::map<std::string, int8_t> &joint_map)
    {
        init_skin(skin, my_skin);
        ASSERT(skin.joints_count > 0, "Skin has not joints!");

        const cgltf_node *skin_root;
        const cgltf_node *scene_root;
        internal::skin_root_node(data, skin, skin_root, scene_root);

        std::vector<std::pair<const cgltf_node *, int8_t>> todo{};
        for (cgltf_size i = 0; i < skin_root->children_count; ++i)
        {
            todo.emplace_back(skin_root->children[i], -1);
        }

        int8_t current_index = 0;
        while (!todo.empty())
        {
            auto[node, parent] = todo.back();
            todo.pop_back();

            if (!internal::in_skin(node, skin))
            {
                continue;
            }

            my_skin.joint_parent_indices[current_index] = parent;
            joint_map[node->name] = current_index;

            for (cgltf_size i = 0; i < node->children_count; ++i)
            {
                todo.emplace_back(node->children[i], current_index);
            }

            current_index++;
        }

        ASSERT(current_index == skin.joints_count, "Failed to load all joints! Hierarchy invalid! {} != {}", current_index, skin.joints_count);

        auto matrix = glm::identity<glm::mat4>();

        if (scene_root->has_matrix)
        {
            memcpy(&matrix, scene_root->matrix, 16 * sizeof(float));
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

        my_skin.joint_global_inverse = glm::inverse(matrix);

        auto &accessor = skin.inverse_bind_matrices;
        auto offset = accessor->offset + accessor->buffer_view->offset;
        for (cgltf_size i = 0; i < skin.joints_count; ++i)
        {
            auto *joint = skin.joints[i];
            const auto index = joint_map[joint->name];
            auto *data = &(reinterpret_cast <char *>(accessor->buffer_view->buffer->data)[offset + i * 16 * sizeof(float)]);

            memcpy(&(my_skin.joint_bind_inverse[index]), data, 16 * sizeof(float));
        }
    }
}
