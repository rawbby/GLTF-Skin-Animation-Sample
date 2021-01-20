#pragma once

#include <components/Skin.hpp>
#include <components/GlSkinnedMesh.hpp>
#include <components/SkinAnimation.hpp>

#include <glm/vec3.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/quaternion.hpp>

#include <memory>
#include <cstdint>

struct SkinAnimator
{
    Skin *skin = nullptr;
    SkinAnimation *anim = nullptr;

    std::unique_ptr<glm::mat4[]> internal_joints{};
    glm::mat4 *joints{};

    int8_t joint_count = 0;

    float current_time = 0.0f;

    void update (float delta)
    {
        current_time += delta;

        while (true)
        {
            for (uint16_t i = 1; i < anim->keyframe_count; ++i)
            {
                if (current_time < anim->timestamps[i])
                {
                    const auto f0 = &(anim->keyframes[(i - 1) * anim->joint_count]);
                    const auto f1 = &(anim->keyframes[(i - 0) * anim->joint_count]);

                    auto parent = glm::identity<glm::mat4>();
                    // auto frame = f0[0].mix(f1[0], current_time);
                    // joints[0] = parent * frame.trs();

                    auto scale = glm::scale(glm::identity<glm::mat4>(), glm::mix(f0[0].scale, f1[0].scale, current_time));
                    auto translate = glm::translate(glm::identity<glm::mat4>(), glm::mix(f0[0].translation, f1[0].translation, current_time));
                    auto rotation = glm::toMat4(glm::slerp(f0[0].rotation, f1[0].rotation, current_time));

                    internal_joints[0] = glm::identity<glm::mat4>();

                    for (int8_t j = 0; j < joint_count; ++j)
                    {
                        const auto parent_index = skin->joint_parent_indices[j] + 1;
                        parent = internal_joints[parent_index];

                        scale = glm::scale(glm::identity<glm::mat4>(), glm::mix(f0[j].scale, f1[j].scale, current_time));
                        translate = glm::translate(glm::identity<glm::mat4>(), glm::mix(f0[j].translation, f1[j].translation, current_time));
                        rotation = glm::toMat4(glm::slerp(f0[j].rotation, f1[j].rotation, current_time));

                        auto jointi = parent * translate * rotation * scale;
                        joints[j] = jointi;
                    }

                    for (int8_t j = 0; j < joint_count; ++j)
                    {
                        joints[j] = skin->joint_global_inverse * joints[j] * skin->joint_bind_inverse[j];
                    }

                    return;
                }
            }

            current_time -= anim->timestamps[anim->keyframe_count - 1];
        }
    }
};
