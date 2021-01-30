#pragma once

#include <util/Assert.hpp>

#include <memory>
#include <cstdint>

namespace model
{
    namespace internal
    {
        void update_joints (glm::mat4 *joints, size_t joint_count, Skin *skin, SkinAnimation *anim, size_t i0, size_t i1, float delta)
        {
            const auto *f0 = &(anim->keyframes[i0 * anim->joint_count]);
            const auto *f1 = &(anim->keyframes[i1 * anim->joint_count]);

            joints[-1] = skin->joint_global_inverse;

            for (size_t j = 0; j < joint_count; ++j)
            {
                const auto &parent = joints[skin->joint_parent_indices[j]];

                const auto trs = f0[j].mix(f1[j], delta).trs();

                joints[j] = parent * trs;
            }

            for (size_t j = 0; j < joint_count; ++j)
                joints[j] *= skin->joint_bind_inverses[j];
        }

        void update_animation (float &current_time, glm::mat4 *joints, size_t joint_count, Skin *skin, SkinAnimation *anim, float delta)
        {
            current_time += delta;

            if (joint_count)
            {
                const auto end_time = anim->timestamps[anim->keyframe_count - 1];

                while (current_time >= end_time)
                    current_time -= end_time;

                while (current_time < 0.0f)
                    current_time += end_time;

                if (current_time < anim->timestamps[0])
                {
                    const auto d = (current_time - 0.0f) / (anim->timestamps[0] - 0.0f);
                    internal::update_joints(joints, joint_count, skin, anim, anim->keyframe_count - 1, 0, d);
                    return;
                }

                for (size_t i = 1; i < anim->keyframe_count; ++i)
                {
                    if (current_time < anim->timestamps[i])
                    {
                        const auto a0 = anim->timestamps[i - 1];
                        const auto a1 = anim->timestamps[i];
                        const auto d = (current_time - a0) / (a1 - a0);

                        internal::update_joints(joints, joint_count, skin, anim, i - 1, i, d);
                        return;
                    }
                }
            }
        }
    }

    struct SkinAnimator
    {
        HeapArray<glm::mat4> internal_joints{};

        Skin *skin = nullptr;
        SkinAnimation *anim = nullptr;

        glm::mat4 *joints{};

        size_t joint_count = 0;
        float current_time = 0.0f;

    public:

        void update (float delta)
        {
            internal::update_animation(current_time, joints, joint_count, skin, anim, delta);
        }
    };

    SkinAnimator create_skin_animator (Skin *skin, SkinAnimation *anim)
    {
        ASSERT(skin, "Animator can not animate nullptr skin!");
        ASSERT(anim, "Animator can not animate nullptr animation!");

        ASSERT(skin->joint_count, "Animator will loop infinite when joint_count is set to zero!");
        ASSERT(skin->joint_count <= 32, "Animator is limited to 32 bones to minimize gpu traffic!");
        ASSERT(skin->joint_count == anim->joint_count, "Skin and SkinAnimation are not compatible!");

        SkinAnimator ator;
        ator.skin = skin;
        ator.anim = anim;
        ator.internal_joints = HeapArray<glm::mat4>(skin->joint_count + 1);
        ator.joints = &(ator.internal_joints[1]);
        ator.joint_count = skin->joint_count;
        ator.current_time = 0.0f;
        return ator;
    }
}
