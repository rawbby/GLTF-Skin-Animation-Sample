#pragma once

#include <model/Types.hpp>

#include <glm/vec3.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

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
                return glm::translate(glm::identity<glm::mat4>(), translation) * glm::toMat4(rotation) * glm::scale(glm::identity<glm::mat4>(), scale);
            }
        };

        std::unique_ptr<TRS[]> keyframes{};
        std::unique_ptr<float[]> timestamps{};

        size_t keyframe_count = 0;
        size_t joint_count = 0;
    };
}
