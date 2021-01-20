#pragma once

#ifndef CGLTF_IMPLEMENTATION
#define CGLTF_IMPLEMENTATION
#include <cgltf.h>
#endif

#include <util/Assert.hpp>

#include <components/Skin.hpp>
#include <components/SkinAnimation.hpp>
#include <components/GlSkinnedMesh.hpp>
#include <components/SkinnedMesh.hpp>

#include <gltf/LoadSkinnedMesh.hpp>
#include <gltf/LoadSkin.hpp>
#include <gltf/LoadSkinAnimation.hpp>
#include <gltf/LoadGlSkinnedMesh.hpp>

namespace gltf
{
    void load_model (GlSkinnedMesh &my_mesh, Skin &my_skin, SkinAnimation &my_anim, std::string_view path)
    {
        cgltf_options options{};
        cgltf_data *data = nullptr;

        ASSERT(cgltf_result_success == cgltf_parse_file(&options, path.data(), &data), "Failed to parse gltf file!");
        ASSERT(cgltf_result_success == cgltf_load_buffers(&options, data, path.data()), "Failed to load gltf buffers!");
        ASSERT(cgltf_result_success == cgltf_validate(data), "Failed to validate gltf data!");

        ASSERT(data->meshes_count == 1, "Only one mesh is supported at the moment! {} are present", data->meshes_count);
        ASSERT(data->skins_count == 1, "Only one skin is supported at the moment! {} are present", data->skins_count);
        ASSERT(data->animations_count == 1, "Only one animation is supported at the moment! {} are present", data->animations_count);

        const auto &mesh = data->meshes[0];
        const auto &skin = data->skins[0];
        const auto &anim = data->animations[0];

        SkinnedMesh mesh_buffer;
        load_mesh(mesh, mesh_buffer);
        load_gl_mesh(mesh_buffer, my_mesh);

        std::map<std::string, uint8_t> joint_map{};
        load_skin(skin, *data, my_skin, joint_map); // TODO maybe data us not required here!
        load_anim(anim, skin, my_anim, joint_map);

        cgltf_free(data);
    }
}
