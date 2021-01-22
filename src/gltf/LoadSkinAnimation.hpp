#pragma once

#ifndef CGLTF_IMPLEMENTATION
#define CGLTF_IMPLEMENTATION
#include <cgltf.h>
#endif

#include <gltf/Types.hpp>

#include <util/Assert.hpp>
#include <model/SkinAnimation.hpp>

namespace gltf
{
    void init_anim (cgltf_animation &anim, cgltf_skin &skin, model::SkinAnimation &my_anim)
    {
        using TRS = model::SkinAnimation::TRS;

        ASSERT(anim.channels_count, "Animation needs to have channels!");
        ASSERT(anim.channels[0].sampler->input->count, "Animation needs to have keyframes!");
        ASSERT(skin.joints_count, "Skin needs to have joints!");

        my_anim.joint_count = skin.joints_count;
        my_anim.keyframe_count = anim.channels[0].sampler->input->count;

        my_anim.timestamps = std::make_unique<float[]>(my_anim.keyframe_count);
        my_anim.keyframes = std::make_unique<TRS[]>(my_anim.joint_count * my_anim.keyframe_count);
    }

    void load_anim (cgltf_animation &anim, cgltf_skin &skin, model::SkinAnimation &my_anim, internal::joint_map_t &joint_map)
    {
        init_anim(anim, skin, my_anim);

        auto *input_accessor = anim.channels[0].sampler->input;
        for (const auto &[i, data] : internal::Accessor<float>{input_accessor})
        {
            my_anim.timestamps[i] = *data;
        }

        for (size_t i = 0; i < anim.channels_count; ++i)
        {
            auto &channel = anim.channels[i];
            auto *output_accessor = channel.sampler->output;
            auto joint_index = joint_map.at(channel.target_node);

            ASSERT(input_accessor == channel.sampler->input, "Only support animations with same timestamp accessor for every channel!");

            switch (channel.target_path)
            {
                case cgltf_animation_path_type_rotation:

                    for (const auto &[j, data] : internal::Accessor<glm::quat>{output_accessor})
                    {
                        my_anim.keyframes[joint_index + j * skin.joints_count].rotation = *data;
                    }
                    break;

                case cgltf_animation_path_type_translation:

                    for (const auto &[j, data] : internal::Accessor<glm::vec3>{output_accessor})
                    {
                        my_anim.keyframes[joint_index + j * skin.joints_count].translation = *data;
                    }
                    break;

                case cgltf_animation_path_type_scale:

                    for (const auto &[j, data] : internal::Accessor<glm::vec3>{output_accessor})
                    {
                        my_anim.keyframes[joint_index + j * skin.joints_count].scale = *data;
                    }
                    break;

                default:
                ASSERT(0, "Unsupported channel path {}", channel.target_path);
            }
        }
    }
}
