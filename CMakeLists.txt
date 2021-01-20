cmake_minimum_required(VERSION 3.18)

project("GLTF - Skin" LANGUAGES CXX)

include(cmake/set_compile_options.cmake)
include(cmake/set_compile_warnings.cmake)

include(cmake/conan.cmake)
conan_add_remote(NAME bincrafters URL https://api.bintray.com/conan/bincrafters/public-conan)
conan_cmake_run(REQUIRES
        glew/2.1.0
        glm/0.9.9.8
        sfml/2.5.1@bincrafters/stable
        spdlog/1.8.1
        cgltf/1.9
        BASIC_SETUP CMAKE_TARGETS BUILD missing)

set(THIS gltf-skin)

add_executable(${THIS})
set_compile_options(${THIS})
set_compile_warnings(${THIS})

target_include_directories(${THIS} PRIVATE src)

target_sources(${THIS} PRIVATE

        src/glsl/vs.glsl.hpp
        src/glsl/fs.glsl.hpp

        src/components/GlSkinnedMesh.hpp
        src/components/Skin.hpp
        src/components/SkinAnimation.hpp
        src/components/SkinAnimator.hpp
        src/components/SkinnedMesh.hpp

        src/gltf/GLTFLoader.hpp
        src/gltf/LoadSkin.hpp
        src/gltf/LoadSkinAnimation.hpp
        src/gltf/LoadSkinnedMesh.hpp

        src/util/Assert.hpp
        src/util/GLUtils.hpp
        src/util/SfmlComponents.hpp

        src/Main.cpp)

target_link_libraries(${THIS} PRIVATE
        $<${WIN32}:opengl32>
        $<$<NOT:${WIN32}>:GL>
        CONAN_PKG::glew
        CONAN_PKG::glm
        CONAN_PKG::sfml
        CONAN_PKG::spdlog
        CONAN_PKG::cgltf)
