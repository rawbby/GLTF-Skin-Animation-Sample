#pragma once

#include <model/SkinAnimator.hpp>
#include <util/Assert.hpp>

#include <memory>
#include <cstdint>

namespace model
{
    namespace internal
    {
        //@formatter:off
        void update_blend_animation (float *current_time,
                                     glm::mat4 *joints, glm::mat4 *joints_buffer,
                                     size_t joint_count,
                                     Skin *skin,
                                     SkinAnimation **anim,
                                     size_t *animation_indices,
                                     float *animation_weights,
                                     float delta)
        {
            internal::update_animation(current_time[0], joints_buffer, joint_count, skin, anim[animation_indices[0]], delta);
            for (size_t j = 0; j < joint_count; ++j)
                joints[j] = joints_buffer[j] * animation_weights[0];

            internal::update_animation(current_time[1], joints_buffer, joint_count, skin, anim[animation_indices[1]], delta);
            for (size_t j = 0; j < joint_count; ++j)
                joints[j] += joints_buffer[j] * animation_weights[1];

            internal::update_animation(current_time[2], joints_buffer, joint_count, skin, anim[animation_indices[2]], delta);
            for (size_t j = 0; j < joint_count; ++j)
                joints[j] += joints_buffer[j] * animation_weights[2];
        }
        //@formatter:on
    }

    struct SkinBlendAnimator
    {
        std::unique_ptr<glm::mat4[]> internal_joints{};

        Skin *skin = nullptr;
        SkinAnimation **anim = nullptr;

        size_t animation_indices[3] = {0, 0, 0};
        float animation_weights[3] = {1.0f, 0.0f, 0.0f};
        float current_time[3] = {0.0f, 0.0f, 0.0f};

        glm::mat4 *joints_buffer = nullptr;
        glm::mat4 *joints = nullptr;

        size_t animation_count = 0;
        size_t joint_count = 0;

    public:

        void update_blend (size_t ai0, size_t ai1, size_t ai2, float aw0, float aw1, float aw2)
        {
            ASSERT(aw0 + aw1 + aw2 <= 1.01f, "Invalid animation weights passed!");
            ASSERT(aw0 + aw1 + aw2 >= 0.99f, "Invalid animation weights passed!");

            ASSERT(ai0 < animation_count, "Invalid animation index passed!");
            ASSERT(ai1 < animation_count, "Invalid animation index passed!");
            ASSERT(ai2 < animation_count, "Invalid animation index passed!");

            animation_indices[0] = ai0;
            animation_indices[1] = ai1;
            animation_indices[2] = ai2;

            animation_weights[0] = aw0;
            animation_weights[1] = aw1;
            animation_weights[2] = aw2;
        }
    };

    void update_blend_animation (SkinBlendAnimator ator, float delta)
    {
        //formatter:off
        internal::update_blend_animation
        (
                ator.current_time,
                ator.joints,
                ator.joints_buffer,
                ator.joint_count,
                ator.skin,
                ator.anim,
                ator.animation_indices,
                ator.animation_weights,
                delta
        );
        //formatter:on
    }

    SkinBlendAnimator create_skin_blend_animator (Skin *skin, SkinAnimation **anim, size_t animation_count)
    {
        ASSERT(skin, "Animator can not animate nullptr skin!");
        ASSERT(anim, "Animator can not animate nullptr animation!");

        ASSERT(skin->joint_count, "Animator will loop infinite when joint_count is set to zero!");
        ASSERT(skin->joint_count == anim[0]->joint_count, "Skin and SkinAnimation are not compatible!");

        size_t internal_joint_size = static_cast<size_t> (skin->joint_count) + 1;

        SkinBlendAnimator ator;
        ator.skin = skin;
        ator.anim = anim;

        ator.animation_indices[0] = 0;
        ator.animation_indices[1] = 0;
        ator.animation_indices[2] = 0;

        ator.animation_weights[0] = 1.0f;
        ator.animation_weights[1] = 0.0f;
        ator.animation_weights[2] = 0.0f;

        ator.current_time[0] = 0.0f;
        ator.current_time[1] = 0.0f;
        ator.current_time[2] = 0.0f;

        ator.internal_joints = std::make_unique<glm::mat4[]>(internal_joint_size * 2);

        ator.joints = &(ator.internal_joints[0 * internal_joint_size + 1]);
        ator.joints_buffer = &(ator.internal_joints[1 * internal_joint_size + 1]);

        ator.animation_count = animation_count;
        ator.joint_count = skin->joint_count;

        return ator;
    }
}
