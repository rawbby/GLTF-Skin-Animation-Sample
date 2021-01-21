#pragma once

#ifndef CGLTF_IMPLEMENTATION
#define CGLTF_IMPLEMENTATION
#include <cgltf.h>
#endif

#include <gltf/Types.hpp>

#include <util/Assert.hpp>

#include <model/Skin.hpp>
#include <model/SkinAnimation.hpp>
#include <model/SkinnedMesh.hpp>

#include <gltf/LoadSkinnedMesh.hpp>
#include <gltf/LoadSkin.hpp>
#include <gltf/LoadSkinAnimation.hpp>

namespace gltf
{
    void load_model (model::Skin &my_skin, model::SkinnedMesh &my_mesh, model::SkinAnimation &my_anim, std::string_view path)
    {
        cgltf_options options{};
        cgltf_data *data = nullptr;

        ASSERT(cgltf_result_success == cgltf_parse_file(&options, path.data(), &data), "Failed to parse gltf file!");
        ASSERT(cgltf_result_success == cgltf_load_buffers(&options, data, path.data()), "Failed to load gltf buffers!");
        ASSERT(cgltf_result_success == cgltf_validate(data), "Failed to validate gltf data!");

        ASSERT(data->meshes_count == 1, "Only one mesh is supported at the moment! {} are present", data->meshes_count);
        ASSERT(data->skins_count == 1, "Only one skin is supported at the moment! {} are present", data->skins_count);
        ASSERT(data->animations_count == 1, "Only one animation is supported at the moment! {} are present", data->animations_count);

        auto &mesh = data->meshes[0];
        auto &skin = data->skins[0];
        auto &anim = data->animations[0];

        internal::joint_map_t joint_map{};
        load_skin(skin, *data, my_skin, joint_map);
        load_anim(anim, skin, my_anim, joint_map);
        load_mesh(mesh, my_mesh, skin, joint_map);

        cgltf_free(data);
    }
}
