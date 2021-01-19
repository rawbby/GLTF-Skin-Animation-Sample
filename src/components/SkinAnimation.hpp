#pragma once

#include <glm/vec3.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <memory>
#include <cstdint>

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
                return { glm::slerp (rotation,    other.rotation,    delta),
                         glm::mix   (translation, other.translation, delta),
                         glm::mix   (scale,       other.scale,       delta)};
                //@formatter:on
        }

        [[nodiscard]] glm::mat4 t () const
        {
            return glm::translate(glm::identity<glm::mat4>(), translation);
        }

        [[nodiscard]] glm::mat4 r () const
        {
            return glm::toMat4(rotation);
        }

        [[nodiscard]] glm::mat4 s () const
        {
            return glm::scale(glm::identity<glm::mat4>(), scale);
        }

        [[nodiscard]] glm::mat4 trs () const
        {
            return t() * r() * s();
        }
    };

    std::unique_ptr<TRS[]> keyframes{};
    std::unique_ptr<float[]> timestamps{};

    uint16_t keyframe_count = 0;
    uint16_t joint_count = 0;
};
