#pragma once

#ifndef CGLTF_IMPLEMENTATION
#define CGLTF_IMPLEMENTATION
#include <cgltf.h>
#endif

#include <gltf/Types.hpp>

#include <util/Assert.hpp>
#include <model/HeapArray.hpp>
#include <model/SkinAnimation.hpp>

namespace gltf
{
    model::HeapArray<model::SkinAnimation> load_animations (cgltf_data *data, const internal::SkinExtra &skin_extra, internal::AnimationsExtra &animations_extra)
    {
        using namespace model;

        const auto &skin = data->skins[skin_extra.skin_index];
        const auto &joint_map = skin_extra.joint_map;

        ASSERT(data->animations_count, "Can not load animations, if there are no animations at all!");
        animations_extra.animation_count = data->animations_count;

        auto my_anims = HeapArray<SkinAnimation>(data->animations_count);
        for (size_t i = 0; i < data->animations_count; ++i)
        {
            auto &anim = data->animations[i];
            auto &my_anim = my_anims[i];
            animations_extra.animation_map[anim.name] = i;

            ASSERT(anim.channels_count, "Can not load animation without channels!");
            ASSERT(anim.channels[i].sampler->input->count > 1, "Can not load animation with less than two keyframes!");
            my_anim = model::SkinAnimation::prepare(anim.channels[i].sampler->input->count, skin.joints_count);

            auto *input_accessor = anim.channels[0].sampler->input;
            for (const auto &[j, data] : internal::Accessor<float>{input_accessor})
            {
                my_anim.timestamps[j] = *data;
            }

            for (size_t j = 0; j < anim.channels_count; ++j)
            {
                auto &channel = anim.channels[j];
                auto *output_accessor = channel.sampler->output;

                auto joint_index = internal::joint_index(channel.target_node, skin);
                joint_index = joint_map.at(joint_index);

                ASSERT(input_accessor == channel.sampler->input, "Only support animations with same timestamp accessor for every channel!");

                switch (channel.target_path)
                {
                    case cgltf_animation_path_type_rotation:

                        for (const auto &[k, data] : internal::Accessor<glm::quat>{output_accessor})
                        {
                            my_anim.keyframes[joint_index + k * skin.joints_count].rotation = *data;
                        }
                        break;

                    case cgltf_animation_path_type_translation:

                        for (const auto &[k, data] : internal::Accessor<glm::vec3>{output_accessor})
                        {
                            my_anim.keyframes[joint_index + k * skin.joints_count].translation = *data;
                        }
                        break;

                    case cgltf_animation_path_type_scale:

                        for (const auto &[k, data] : internal::Accessor<glm::vec3>{output_accessor})
                        {
                            my_anim.keyframes[joint_index + k * skin.joints_count].scale = *data;
                        }
                        break;

                    default:
                    ASSERT(0, "Unsupported channel path {}", channel.target_path);
                }
            }
        }

        return my_anims;
    }
}
