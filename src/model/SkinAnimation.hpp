#pragma once

#include <model/HeapArray.hpp>

#include <memory>
#include <cstdint>

namespace model
{
    struct SkinAnimation
    {
        struct TRS
        {
            glm::quat rotation{};
            glm::vec3 translation{};
            glm::vec3 scale{};

            [[nodiscard]] TRS mix (const TRS other, float delta) const
            {
                //@formatter:off
                return { .rotation    = glm::slerp (rotation,    other.rotation,    delta),
                         .translation = glm::mix   (translation, other.translation, delta),
                         .scale       = glm::mix   (scale,       other.scale,       delta) };
                //@formatter:on
            }

            [[nodiscard]] glm::mat4 trs () const
            {
                return translate(glm::identity<glm::mat4>(), translation) * glm::toMat4(rotation) * glm::scale(glm::identity<glm::mat4>(), scale);
            }
        };

        HeapArray <TRS> keyframes{};
        HeapArray<float> timestamps{};

        size_t keyframe_count = 0;
        size_t joint_count = 0;

        static SkinAnimation prepare (size_t keyframe_count, size_t joint_count)
        {
            SkinAnimation anim;
            anim.keyframes = HeapArray<TRS>(keyframe_count * joint_count);
            anim.timestamps = HeapArray<float>(keyframe_count);
            anim.keyframe_count = keyframe_count;
            anim.joint_count = joint_count;
            return anim;
        }
    };
}
