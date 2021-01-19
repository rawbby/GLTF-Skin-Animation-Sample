#pragma once

#ifndef CGLTF_IMPLEMENTATION
#define CGLTF_IMPLEMENTATION
#include <cgltf.h>
#endif

#include <util/Assert.hpp>
#include <components/Skin.hpp>

namespace gltf
{
    void init_skin (const cgltf_skin &skin, Skin &my_skin)
    {
        my_skin.joint_global_inverse = glm::identity<glm::mat4>();
        my_skin.joint_count = static_cast<uint8_t>(skin.joints_count);
        my_skin.joint_bind_inverse = std::make_unique<glm::mat4[]>(my_skin.joint_count);
        my_skin.joint_parent_indices = std::make_unique<uint8_t[]>(my_skin.joint_count);
    }

    void load_skin (const cgltf_skin &skin, const cgltf_data &data, Skin &my_skin, std::map<std::string, uint8_t> &joint_map)
    {
        init_skin(skin, my_skin);

        ASSERT(skin.joints_count > 0, "Skin has not joints!");

        uint8_t current_index = 0;
        std::vector<std::pair<cgltf_node *, uint8_t>> todo{};
        todo.emplace_back(skin.joints[0], current_index);

        while (!todo.empty())
        {
            auto[node, parent] = todo.back();
            todo.pop_back();

            my_skin.joint_parent_indices[current_index] = parent;
            joint_map[node->name] = current_index;

            for (cgltf_size i = 0; i < node->children_count; ++i)
            {
                todo.emplace_back(node->children[i], current_index);
            }

            current_index++;
        }

        ASSERT(data.scenes_count == 1, "Only one scene is supported at the moment!");
        ASSERT(data.scenes[0].nodes_count == 1, "Only one root node is supported at the moment!");
        const auto *root_node = data.scenes[0].nodes[0];

        glm::mat4 matrix = glm::identity<glm::mat4>();

        if (root_node->has_matrix)
        {
            memcpy(&matrix, root_node->matrix, 16 * sizeof(float));
        }
        if (root_node->has_translation)
        {
            glm::vec3 translation(root_node->translation[0], root_node->translation[1], root_node->translation[2]);
            matrix = glm::translate(matrix, translation);
        }
        if (root_node->has_rotation)
        {
            glm::quat rotation(root_node->rotation[0], root_node->rotation[1], root_node->rotation[2], root_node->rotation[3]);
            matrix = glm::toMat4(rotation) * matrix;
        }
        if (root_node->has_scale)
        {
            glm::vec3 scale(root_node->scale[0], root_node->scale[1], root_node->scale[2]);
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
