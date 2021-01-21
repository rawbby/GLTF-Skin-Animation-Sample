#pragma once

#include <model/Types.hpp>

#include <model/Skin.hpp>
#include <model/GlSkinnedMesh.hpp>
#include <model/SkinAnimation.hpp>

#include <glm/vec3.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/quaternion.hpp>

#include <memory>
#include <cstdint>

namespace model
{
    struct SkinAnimator
    {
        Skin *skin = nullptr;
        SkinAnimation *anim = nullptr;

        std::unique_ptr<glm::mat4[]> internal_joints{};
        glm::mat4 *joints{};

        size_t joint_count = 0;
        float current_time = 0.0f;

    private:

        void update (size_t i0, size_t i1, float delta)
        {
            const auto *f0 = &(anim->keyframes[i0 * anim->joint_count]);
            const auto *f1 = &(anim->keyframes[i1 * anim->joint_count]);

            internal_joints[0] = skin->joint_global_inverse;

            for (size_t j = 0; j < joint_count; ++j)
            {
                const auto parent_index = static_cast<size_t> (skin->joint_parent_indices[j]) + 1;
                const auto &parent = internal_joints[parent_index];

                const auto trs = f0[j].mix(f1[j], delta).trs();

                joints[j] = parent * trs;
            }

            for (size_t j = 0; j < joint_count; ++j)
                joints[j] *= skin->joint_bind_inverses[j];
        }

    public:

        void update (float delta)
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
                    const auto t0 = 0.0f;
                    const auto t1 = anim->timestamps[0];
                    const auto d = (current_time - t0) / (t1 - t0);

                    ASSERT(d <= 1.0f, "invalid delta");
                    ASSERT(d >= 0.0f, "invalid delta");

                    update(anim->keyframe_count - 1, 0, d);
                    return;
                }

                for (size_t i = 1; i < anim->keyframe_count; ++i)
                {
                    if (current_time < anim->timestamps[i])
                    {
                        const auto t0 = anim->timestamps[i-1];
                        const auto t1 = anim->timestamps[i];
                        const auto d = (current_time - t0) / (t1 - t0);

                        ASSERT(d <= 1.0f, "invalid delta");
                        ASSERT(d >= 0.0f, "invalid delta");

                        update(i - 1, i, d);
                        return;
                    }
                }
            }
        }
    };
}
