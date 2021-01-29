#pragma once

#include <model/SkinBlendAnimator.hpp>

namespace model
{
    namespace internal
    {
        void update_transition (size_t *animation_indices, float *animation_weights, float *transition_delta, size_t anim_index, float delta)
        {
            auto &i0 = animation_indices[0];
            auto &i1 = animation_indices[1];
            auto &i2 = animation_indices[2];

            auto &w0 = animation_weights[0];
            auto &w1 = animation_weights[1];
            auto &w2 = animation_weights[2];

            i2 = i1;
            i1 = i0;
            w2 = w1 + 0.0000001f;
            w1 = w0 + 0.0000001f;

            i0 = anim_index;
            w0 = 0.0f;

            const auto sum = w1 + w2;
            const auto remain = 1.0f - sum;
            w1 += remain * (w1 / sum);
            w2 += remain * (w2 / sum);

            *transition_delta = delta;
        }

        void update_transition_animation (float *current_time, glm::mat4 *joints, glm::mat4 *joints_buffer, size_t joint_count, Skin *skin, SkinAnimation **anim, size_t *animation_indices, float *animation_weights, float *transition_delta, float delta)
        {
            if (*transition_delta > 0.0f)
            {
                delta = delta < *transition_delta ? delta : *transition_delta;

                auto go = delta / *transition_delta;
                animation_weights[0] += go * (1.0f - animation_weights[0]);
                animation_weights[1] -= go * animation_weights[1];
                animation_weights[2] -= go * animation_weights[2];

                *transition_delta -= delta;
            }

            internal::update_blend_animation(current_time, joints, joints_buffer, joint_count, skin, anim, animation_indices, animation_weights, delta);
        }
    }

    struct SkinTransitionAnimator
            : public SkinBlendAnimator
    {
        float transition_delta = 0.0f;
    };

    inline void update_transition (SkinTransitionAnimator &ator, size_t anim_index, float delta)
    {
        internal::update_transition(ator.animation_indices, ator.animation_weights, &ator.transition_delta, anim_index, delta);
    }

    inline void update_transition_animation (SkinTransitionAnimator &ator, float delta)
    {
        internal::update_transition_animation(ator.current_time, ator.joints, ator.joints_buffer, ator.joint_count, ator.skin, ator.anim, ator.animation_indices, ator.animation_weights, &ator.transition_delta, delta);
    }

    SkinTransitionAnimator create_skin_transition_animator (Skin *skin, SkinAnimation **anim, size_t animation_count)
    {
        ASSERT(skin, "Animator can not animate nullptr skin!");
        ASSERT(anim, "Animator can not animate nullptr animation!");

        ASSERT(skin->joint_count, "Animator will loop infinite when joint_count is set to zero!");
        ASSERT(skin->joint_count == anim[0]->joint_count, "Skin and SkinAnimation are not compatible!");

        size_t internal_joint_size = static_cast<size_t> (skin->joint_count) + 1;

        SkinTransitionAnimator ator;
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

        ator.transition_delta = 0.0f;

        return ator;
    }
}
