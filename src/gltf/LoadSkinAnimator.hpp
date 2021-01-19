#pragma once

#ifndef CGLTF_IMPLEMENTATION
#define CGLTF_IMPLEMENTATION

#include <cgltf.h>

#endif

#include <glm/matrix.hpp>

#include <util/Assert.hpp>
#include <components/SkinAnimation.hpp>
#include <components/Skin.hpp>
#include <components/SkinAnimator.hpp>

#include <memory>

namespace gltf
{
    void init_ator (SkinAnimator &my_ator, Skin *my_skin, SkinAnimation *my_anim)
    {
        ASSERT(my_skin->joint_count, "Animator will loop infinite when joint_count is set to zero!");
        ASSERT(static_cast<uint16_t> (my_skin->joint_count) == my_anim->joint_count, "Skin and SkinAnimation are not compatible!");

        my_ator.current_time = 0.0f;
        my_ator.joint_count = my_skin->joint_count;

        my_ator.joints = std::make_unique<glm::mat4[]>(my_ator.joint_count);

        my_ator.anim = my_anim;
        my_ator.skin = my_skin;

        my_ator.update(0.0f);
    }
}
