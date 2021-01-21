#pragma once

#ifndef CGLTF_IMPLEMENTATION
#define CGLTF_IMPLEMENTATION

#include <cgltf.h>

#endif

#include <gltf/Types.hpp>

#include <glm/matrix.hpp>

#include <util/Assert.hpp>
#include <model/SkinAnimation.hpp>
#include <model/Skin.hpp>
#include <model/SkinAnimator.hpp>

#include <memory>

namespace gltf
{
    void init_ator (model::SkinAnimator &my_ator, model::Skin *my_skin, model::SkinAnimation *my_anim)
    {
        ASSERT(my_skin->joint_count, "Animator will loop infinite when joint_count is set to zero!");
        ASSERT(static_cast<uint16_t> (my_skin->joint_count) == my_anim->joint_count, "Skin and SkinAnimation are not compatible!");

        my_ator.current_time = 0.0f;
        my_ator.joint_count = my_skin->joint_count;

        my_ator.internal_joints = std::make_unique<glm::mat4[]>(static_cast<size_t> (my_ator.joint_count) + 1);
        my_ator.joints = &(my_ator.internal_joints[1]);

        my_ator.anim = my_anim;
        my_ator.skin = my_skin;

        my_ator.update(0.0f);
    }
}
