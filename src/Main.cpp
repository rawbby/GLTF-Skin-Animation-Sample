#include <GL/glew.h>
#include <GL/GL.h>

#include <SFML/Graphics.hpp>

#include <util/SfmlComponents.hpp>
#include <util/GLUtils.hpp>

#include <components/SkinnedMesh.hpp>
#include <components/Skin.hpp>
#include <components/SkinAnimation.hpp>
#include <components/SkinAnimator.hpp>

#include <gltf/GLTFLoader.hpp>

int main ()
{
    sf::ContextSettings settings{24, 8, 4, 4, 5, sf::ContextSettings::Default, false};
    sf::Window window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default, settings);
    window.setFramerateLimit(144);

    glewExperimental = GL_TRUE;
    glewInit();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    Skin skin{};
    SkinnedMesh mesh{};
    SkinAnimation anim{};
    gltf::load_model(mesh, skin, anim, "res/c.gltf");

    SkinAnimator ator{};
    gltf::init_ator(ator, &skin, &anim);

    const auto vertex_shader = vertex_shader_from_path("res/vs.glsl");
    const auto fragment_shader = fragment_shader_from_path("res/fs.glsl");
    const auto program = compile_program(vertex_shader, fragment_shader);

    Escape escape{&window};
    Camera camera{};
    Projection projection{800.0f, 600.0f};

    while (window.isOpen())
    {
        ator.update(0.01f);

        glClearColor(0.6f, 0.75f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);
        glBindVertexArray(mesh.vao);

        glUniformMatrix4fv(glGetUniformLocation(program, "u_projection_matrix"), 1, GL_FALSE, glm::value_ptr(projection.matrix() * camera.matrix()));
        glUniformMatrix4fv(glGetUniformLocation(program, "u_model_view_matrix"), 1, GL_FALSE, glm::value_ptr(glm::identity<glm::mat4>()));

        glUniformMatrix4fv(glGetUniformLocation(program, "u_joints"), skin.joint_count, GL_FALSE, glm::value_ptr(*(ator.joints.get())));;

        glDrawElements(GL_TRIANGLES, mesh.index_count, GL_UNSIGNED_SHORT, nullptr);

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
