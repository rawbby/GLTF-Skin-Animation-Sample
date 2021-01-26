#include <GL/glew.h>
#include <GL/GL.h>

#include <glsl/vs.glsl.hpp>
#include <glsl/fs.glsl.hpp>

#include <SFML/Graphics.hpp>

#include <util/SfmlComponents.hpp>
#include <util/GLUtils.hpp>

#include <model/GlSkinnedMesh.hpp>
#include <model/Skin.hpp>
#include <model/SkinAnimation.hpp>
#include <model/SkinAnimator.hpp>
#include <model/SkinBlendAnimator.hpp>

#include <gltf/GLTFLoader.hpp>

#include <cmath>

int main (const int argc, const char **argv)
{
    ASSERT(argc > 1, "Please provide an input file! (.gltf)");

    sf::ContextSettings settings{24, 8, 4, 4, 5, sf::ContextSettings::Default, false};
    sf::Window window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default, settings);
    window.setFramerateLimit(144);

    sf::Clock clock{};

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
    auto ator = model::SkinBlendAnimator::create(&skin, anims.get(), anims_extra.animation_count);

    ator.update(
            anims_extra.animation_map["Wabble"],
            anims_extra.animation_map["move"],
            0,
            0.5f, 0.5f, 0.0f);

    const auto vertex_shader = compile_shader(GL_VERTEX_SHADER, glsl::vs::blend_animator);
    const auto fragment_shader = compile_shader(GL_FRAGMENT_SHADER, glsl::fs::text);
    const auto program = compile_program(vertex_shader, fragment_shader);

    Escape escape{&window};
    Camera camera{};
    Projection projection{800.0f, 600.0f};

    while (window.isOpen())
    {
        ator.update(clock.getElapsedTime().asSeconds());
        clock.restart();

        glClearColor(0.6f, 0.75f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);
        glBindVertexArray(mesh.vao);

        glUniformMatrix4fv(glGetUniformLocation(program, "u_projection_matrix"), 1, GL_FALSE, glm::value_ptr(projection.matrix() * camera.matrix()));
        glUniformMatrix4fv(glGetUniformLocation(program, "u_model_view_matrix"), 1, GL_FALSE, glm::value_ptr(glm::identity<glm::mat4>()));

        glUniform1f(glGetUniformLocation(program, "u_animation_weight_0"), ator.animation_weights[0]);
        glUniform1f(glGetUniformLocation(program, "u_animation_weight_1"), ator.animation_weights[1]);
        glUniform1f(glGetUniformLocation(program, "u_animation_weight_2"), ator.animation_weights[2]);

        glUniformMatrix4fv(glGetUniformLocation(program, "u_animation_joints_0"), skin.joint_count, GL_FALSE, glm::value_ptr(*ator.joints[0]));
        glUniformMatrix4fv(glGetUniformLocation(program, "u_animation_joints_1"), skin.joint_count, GL_FALSE, glm::value_ptr(*ator.joints[1]));
        glUniformMatrix4fv(glGetUniformLocation(program, "u_animation_joints_2"), skin.joint_count, GL_FALSE, glm::value_ptr(*ator.joints[2]));

        glDrawElements(GL_TRIANGLES, mesh.index_count, GL_UNSIGNED_INT, nullptr);

        glUseProgram(GL_NONE);

        glFlush();
        window.display();

        for (sf::Event event{}; window.pollEvent(event);)
        {
            escape.update(event);
            camera.update(event);
            projection.update(event);
        }
    }
}
