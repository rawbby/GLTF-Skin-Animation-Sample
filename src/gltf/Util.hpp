#pragma once

#ifndef CGLTF_IMPLEMENTATION
#define CGLTF_IMPLEMENTATION
#include <cgltf.h>
#endif

#include <gltf/Types.hpp>
#include <iterator>

namespace gltf::internal
{
    glm::mat4 model_view_matrix (cgltf_node *node)
    {
        auto matrix = glm::identity<glm::mat4>();

        if (node->has_matrix)
        {
            memcpy(&matrix, node->matrix, sizeof(glm::mat4));
        }
        if (node->has_translation)
        {
            matrix = glm::translate(matrix, {node->translation[0], node->translation[1], node->translation[2]});
        }
        if (node->has_rotation)
        {
            matrix = glm::toMat4(glm::quat{node->rotation[0], node->rotation[1], node->rotation[2], node->rotation[3]}) * matrix;
        }
        if (node->has_scale)
        {
            matrix = glm::scale(matrix, {node->scale[0], node->scale[1], node->scale[2]});
        }

        return matrix;
    }

    size_t component_size (cgltf_accessor *accessor)
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

    size_t component_count (cgltf_accessor *accessor)
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

    size_t accessor_offset (cgltf_accessor *accessor)
    {
        return accessor->offset + accessor->buffer_view->offset;
    }

    size_t accessor_stride (cgltf_accessor *accessor)
    {
        return accessor->stride ? accessor->stride : component_size(accessor) * component_count(accessor);
    }

    template <typename T>
    struct Accessor
    {
        cgltf_accessor *accessor = nullptr;

        class Iterator
        {
        public:

            using iterator_category = std::input_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = std::pair<size_t, T *>;
            using pointer = value_type *;
            using reference = value_type &;

        private:

            size_t m_index = 0;
            size_t m_offset = 0;
            size_t m_stride = 0;
            char *m_data = nullptr;

        private:

            explicit Iterator (cgltf_accessor *accessor, size_t index = 0)
                    : m_index(index)
            {
                ASSERT(accessor, "Invalid accessor passed (nullptr)!");

                m_offset = accessor_offset(accessor);
                m_stride = accessor_stride(accessor);
                m_data = reinterpret_cast<char *> (accessor->buffer_view->buffer->data);

                ASSERT(m_stride >= sizeof(T), "Invalid stride in accessor!");
            }

        public:

            static Iterator begin (cgltf_accessor *accessor)
            {
                return Iterator{accessor};
            }

            static Iterator end (cgltf_accessor *accessor)
            {
                return Iterator{accessor, accessor->count};
            }

            value_type operator* () const
            {
                return value_type{m_index, reinterpret_cast<T *>(&(m_data[m_offset + m_index * m_stride]))};
            }

            Iterator &operator++ ()
            {
                m_index++;
                return *this;
            }

            Iterator operator++ (int) // NOLINT(cert-dcl21-cpp)
            {
                Iterator tmp = *this;
                ++(*this);
                return tmp;
            }

            friend bool operator== (const Iterator &a, const Iterator &b)
            {
                return a.m_data == b.m_data && a.m_index == b.m_index;
            };

            friend bool operator!= (const Iterator &a, const Iterator &b)
            {
                return a.m_data != b.m_data || a.m_index != b.m_index;
            };
        };

        Iterator begin () const
        {
            return Iterator::begin(accessor);
        }

        Iterator end () const
        {
            return Iterator::end(accessor);
        }
    };

    size_t joint_index (const cgltf_node *node, const cgltf_skin &skin)
    {
        for (size_t j = 0; j < skin.joints_count; ++j)
        {
            if (skin.joints[j] == node)
            {
                return j;
            }
        }
        return -1;
    }

    void skin_root_node (const cgltf_data &data, const cgltf_skin &skin, cgltf_node *&skin_root, cgltf_node *&scene_root)
    {
        std::vector<cgltf_node *> todo{};

        for (size_t i = 0; i < data.scenes_count; ++i)
        {
            auto &scene = data.scene[i];

            for (size_t j = 0; j < scene.nodes_count; ++j)
            {
                todo.push_back(scene.nodes[j]);

                while (!todo.empty())
                {
                    auto *node = todo.back();
                    todo.pop_back();

                    for (size_t k = 0; k < node->children_count; ++k)
                    {
                        auto *child = node->children[k];

                        if (joint_index(child, skin) != static_cast<size_t> (-1))
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
        for (size_t i = 0; i < primitive.attributes_count; ++i)
        {
            auto &attribute = primitive.attributes[i];

            if (attribute.type == type && attribute.index == index)
            {
                return &attribute;
            }
        }

        ASSERT(0, "Mesh does not provide attribute {} with m_index {}!", type, index);
    }
}
