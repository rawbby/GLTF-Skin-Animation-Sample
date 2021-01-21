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

        ASSERT(anim.channels_count > 0, "invalid Animation no channels");

        my_anim.joint_count = skin.joints_count;
        my_anim.keyframe_count = anim.channels[0].sampler->input->count;

        my_anim.timestamps = std::make_unique<float[]>(my_anim.keyframe_count);
        my_anim.keyframes = std::make_unique<TRS[]>(my_anim.joint_count * my_anim.keyframe_count);
    }

    void load_anim (cgltf_animation &anim, cgltf_skin &skin, model::SkinAnimation &my_anim, internal::joint_map_t &joint_map)
    {
        init_anim(anim, skin, my_anim);

        ASSERT(anim.channels_count > 0, "invalid Animation no channels");
        auto *input_accessor = anim.channels[0].sampler->input;
        auto input_offset = input_accessor->offset + input_accessor->buffer_view->offset;

        for (cgltf_size i = 0; i < input_accessor->count; ++i)
        {
            auto *time_stamp_data = &(reinterpret_cast<char *>(input_accessor->buffer_view->buffer->data)[input_offset]);
            auto *timestamp = reinterpret_cast<float *>(time_stamp_data);
            my_anim.timestamps[i] = timestamp[i];
        }

        for (cgltf_size channel_j = 0; channel_j < anim.channels_count; ++channel_j)
        {
            auto &channel = anim.channels[channel_j];
            auto index = joint_map.at(channel.target_node);

            auto *output_accessor = channel.sampler->output;
            auto output_offset = output_accessor->offset + output_accessor->buffer_view->offset;

            ASSERT(channel.sampler->input == input_accessor, "only support anim with same timestamps");

            for (cgltf_size i = 0; i < output_accessor->count; ++i)
            {
                auto *animation_data = &(reinterpret_cast<char *>(output_accessor->buffer_view->buffer->data)[output_offset]);
                switch (channel.target_path)
                {
                    case cgltf_animation_path_type_rotation :
                    {
                        ASSERT(output_accessor->stride == 16, "stride bigger zero ");
                        auto *quat_data = reinterpret_cast<glm::quat *>(animation_data);
                        my_anim.keyframes[i * skin.joints_count + index].rotation = quat_data[i];
                        break;
                    }
                    case cgltf_animation_path_type_translation :
                    {
                        ASSERT(output_accessor->stride == 12, "stride bigger zero ");
                        auto *vec3_data = reinterpret_cast<glm::vec3 *>(animation_data);
                        my_anim.keyframes[i * skin.joints_count + index].translation = vec3_data[i];
                        break;
                    }
                    case cgltf_animation_path_type_scale :
                    {
                        ASSERT(output_accessor->stride == 12, "stride bigger zero ");
                        auto *vec3_data = reinterpret_cast<glm::vec3 *>(animation_data);
                        my_anim.keyframes[i * skin.joints_count + index].scale = vec3_data[i];
                        break;
                    }
                    default:
                    ASSERT(0, "unsupported channel path");
                }
            }
        }
    }
}
