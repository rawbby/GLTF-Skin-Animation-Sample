#include <GL/glew.h>
#include <GL/GL.h>

#include <glsl/vs.glsl.hpp>
#include <glsl/fs.glsl.hpp>

#include <SFML/Graphics.hpp>

#include <util/SfmlComponents.hpp>
#include <util/GLUtils.hpp>
#include <model/HeapArray.hpp>

#include <model/GlSkinnedMesh.hpp>
#include <model/Skin.hpp>
#include <model/SkinAnimation.hpp>
#include <model/SkinTransitionAnimator.hpp>

#include <gltf/GLTFLoader.hpp>

int main (const int argc, const char **argv)
{
    ASSERT(argc > 1, "Please provide an input file! (.gltf)");

    sf::ContextSettings settings{24, 8, 4, 4, 5, sf::ContextSettings::Default, false};
    sf::Window window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default, settings);
    window.setFramerateLimit(144);

    glewExperimental = GL_TRUE;
    glewInit();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    auto data = gltf::load_gltf(argv[1]);

    gltf::internal::SkinExtra skin_extra{};
    auto skin = gltf::load_single_skin(data.get(), skin_extra);
    auto mesh_buffer = gltf::load_single_mesh(data.get(), skin_extra);

    gltf::internal::AnimationsExtra anims_extra{};
    auto anims = gltf::load_animations(data.get(), skin_extra, anims_extra);

    auto mesh = model::GlSkinnedMesh::fromSkinnedMesh(mesh_buffer);
    auto ator = model::create_skin_transition_animator(&skin, anims.data(), anims_extra.animation_count);

    update_transition(ator, anims_extra.animation_map["Wabble"], 5.0f);

    const auto vertex_shader = compile_shader(GL_VERTEX_SHADER, glsl::vs::animator);
    const auto fragment_shader = compile_shader(GL_FRAGMENT_SHADER, glsl::fs::text);
    const auto program = compile_program(vertex_shader, fragment_shader);

    Escape escape{&window};
    AnyKey any_key{};
    Camera camera{};
    Projection projection{800.0f, 600.0f};
    sf::Clock clock{};

    while (window.isOpen())
    {
        if (any_key.any_pressed())
        {
            if (ator.animation_indices[0] == 0)
                update_transition(ator, 1, 5.0f);
            else
                update_transition(ator, 0, 5.0f);
        }

        const auto delta = clock.getElapsedTime().asSeconds();
        update_transition_animation(ator, delta);
        clock.restart();

        glClearColor(0.6f, 0.75f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);
        glBindVertexArray(mesh.vao);

        glUniformMatrix4fv(glGetUniformLocation(program, "u_projection_matrix"), 1, GL_FALSE, glm::value_ptr(projection.matrix() * camera.matrix()));
        glUniformMatrix4fv(glGetUniformLocation(program, "u_model_view_matrix"), 1, GL_FALSE, glm::value_ptr(glm::identity<glm::mat4>()));
        glUniformMatrix4fv(glGetUniformLocation(program, "u_joints"), skin.joint_count, GL_FALSE, glm::value_ptr(*ator.joints));

        glDrawElements(GL_TRIANGLES, mesh.index_count, GL_UNSIGNED_INT, nullptr);

        glUseProgram(GL_NONE);

        glFlush();
        window.display();

        for (sf::Event event{}; window.pollEvent(event);)
        {
            escape.update(event);
            any_key.update(event);
            camera.update(event);
            projection.update(event);
        }
    }
}
