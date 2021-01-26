#pragma once

#include <util/Assert.hpp>

#include <memory>
#include <cstdint>

namespace model
{
    struct SkinBlendAnimator
    {
        Skin *skin = nullptr;

        SkinAnimation *anim = nullptr; // array of animations
        size_t animation_indices[3] = {0, 0, 0};
        float animation_weights[3] = {1.0f, 0.0f, 0.0f};

        std::unique_ptr<glm::mat4[]> internal_joints{};
        glm::mat4 *joints[3]{};

        size_t animation_count = 0;
        size_t joint_count = 0;
        float current_time[3] = {0.0f, 0.0f, 0.0f};

    public:

        static SkinBlendAnimator create (Skin *skin, SkinAnimation *anim, size_t animation_count)
        {
            ASSERT(skin, "Animator can not animate nullptr skin!");
            ASSERT(anim, "Animator can not animate nullptr animation!");

            ASSERT(skin->joint_count, "Animator will loop infinite when joint_count is set to zero!");
            ASSERT(skin->joint_count == anim->joint_count, "Skin and SkinAnimation are not compatible!");

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

            ator.internal_joints = std::make_unique<glm::mat4[]>(internal_joint_size * 3);

            ator.joints[0] = &(ator.internal_joints[0 * internal_joint_size + 1]);
            ator.joints[1] = &(ator.internal_joints[1 * internal_joint_size + 1]);
            ator.joints[2] = &(ator.internal_joints[2 * internal_joint_size + 1]);

            ator.animation_count = animation_count;
            ator.joint_count = skin->joint_count;

            ator.current_time[0] = 0.0f;
            ator.current_time[1] = 0.0f;
            ator.current_time[2] = 0.0f;

            return ator;
        }

    private:

        void update_joints (size_t ji, size_t ai, size_t i0, size_t i1, float delta)
        {
            const auto *f0 = &(anim[ai].keyframes[i0 * anim[ai].joint_count]);
            const auto *f1 = &(anim[ai].keyframes[i1 * anim[ai].joint_count]);

            joints[ji][-1] = skin->joint_global_inverse;

            for (size_t j = 0; j < joint_count; ++j)
            {
                const auto &parent = joints[ji][skin->joint_parent_indices[j]];

                const auto trs = f0[j].mix(f1[j], delta).trs();

                joints[ji][j] = parent * trs;
            }

            for (size_t j = 0; j < joint_count; ++j)
                joints[ji][j] *= skin->joint_bind_inverses[j];
        }

        void update_animation (size_t ji, size_t ai, float delta)
        {
            current_time[ji] += delta;
            const auto end_time = anim[ai].timestamps[anim[ai].keyframe_count - 1];

            while (current_time[ji] >= end_time)
                current_time[ji] -= end_time;

            while (current_time[ji] < 0.0f)
                current_time[ji] += end_time;

            if (current_time[ji] < anim[ai].timestamps[0])
            {
                const auto t0 = 0.0f;
                const auto t1 = anim[ai].timestamps[0];
                const auto d = (current_time[ji] - t0) / (t1 - t0);

                ASSERT(d <= 1.0f, "invalid delta");
                ASSERT(d >= 0.0f, "invalid delta");

                update_joints(ji, ai, anim[ai].keyframe_count - 1, 0, d);
                return;
            }

            for (size_t i = 1; i < anim[ai].keyframe_count; ++i)
            {
                if (current_time[ji] < anim[ai].timestamps[i])
                {
                    const auto t0 = anim[ai].timestamps[i - 1];
                    const auto t1 = anim[ai].timestamps[i];
                    const auto d = (current_time[ji] - t0) / (t1 - t0);

                    ASSERT(d <= 1.0f, "invalid delta");
                    ASSERT(d >= 0.0f, "invalid delta");

                    update_joints(ji, ai, i - 1, i, d);
                    return;
                }
            }
        }

    public:

        void update (size_t ai0, size_t ai1, size_t ai2, float aw0, float aw1, float aw2)
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

        void update (float delta)
        {
            update_animation(0, animation_indices[0], delta);
            update_animation(1, animation_indices[1], delta);
            update_animation(2, animation_indices[2], delta);
        }
    };
}
