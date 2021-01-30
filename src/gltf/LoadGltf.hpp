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
    std::unique_ptr<cgltf_data, void (*) (cgltf_data *)> load_gltf (std::string_view path)
    {
        cgltf_options options{};
        cgltf_data *raw_data = nullptr;

        ASSERT(cgltf_result_success == cgltf_parse_file(&options, path.data(), &raw_data), "Failed to parse gltf file!");
        std::unique_ptr<cgltf_data, void (*) (cgltf_data *)> data{raw_data, cgltf_free};

        ASSERT(cgltf_result_success == cgltf_load_buffers(&options, raw_data, path.data()), "Failed to load gltf buffers!");
        ASSERT(cgltf_result_success == cgltf_validate(raw_data), "Failed to validate gltf raw_data!");

        return data;
    }
}
