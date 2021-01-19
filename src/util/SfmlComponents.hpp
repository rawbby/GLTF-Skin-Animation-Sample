#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <spdlog/spdlog.h>

#include <SFML/Graphics.hpp>

class Escape
{
private:

    sf::Window *m_window;

public:

    explicit Escape (sf::Window *window)
            : m_window(window)
    {
    }

    bool update (sf::Event event)
    {
        if (event.type == sf::Event::Closed)
        {
            m_window->close();
            return true;
        }
        return false;
    }
};

class Camera
{
private:

    glm::vec3 m_center;
    glm::vec3 m_up;
    glm::mat4 m_matrix;

private:

    float m_rotation_y;
    float m_rotation_x;
    float m_distance;

    bool mouse_pressed;
    sf::Vector2f mouse_position;

private:

    [[nodiscard]] glm::vec3 position () const
    {
        return glm::rotateY(glm::rotateX(glm::vec3{0.0f, 0.0f, m_distance}, glm::radians(m_rotation_x)), glm::radians(m_rotation_y));
        // return glm::rotateY(glm::rotateX(glm::vec3{0.0f, 0.0f, -6.0f}, m_rotation_x), m_rotation_y);
    }

public:

    explicit Camera ()
            : m_center(0.0f, 5.0f, 0.0f)
            , m_up(0.0f, 1.0f, 0.0f)
            , m_rotation_y(0.0f)
            , m_rotation_x(0.0f)
            , m_distance(6.0f)
            , mouse_pressed(false)
            , mouse_position()
    {
        m_matrix = glm::lookAt(position(), m_center, m_up);
    }

    bool update (sf::Event event)
    {
        if (mouse_pressed)
        {
            m_rotation_y += static_cast<float> (sf::Mouse::getPosition().x - mouse_position.x) * +0.5f;
            m_rotation_x += static_cast<float> (sf::Mouse::getPosition().y - mouse_position.y) * -0.5f;

            //@formatter:off
            while (m_rotation_y < -180.0f) m_rotation_y += 360.0f;
            while (m_rotation_y > 180.0f)  m_rotation_y -= 360.0f;
            m_rotation_x = m_rotation_x > 80.0f ? 80.0f : m_rotation_x;
            m_rotation_x = m_rotation_x < -80.0f ? -80.0f : m_rotation_x;
            //@formatter:on

            m_matrix = glm::lookAt(position(), m_center, m_up);

            mouse_position.x = sf::Mouse::getPosition().x;
            mouse_position.y = sf::Mouse::getPosition().y;
        }

        if (event.type == sf::Event::MouseButtonPressed)
        {
            mouse_pressed = true;
            mouse_position.x = sf::Mouse::getPosition().x;
            mouse_position.y = sf::Mouse::getPosition().y;
        }

        if (event.type == sf::Event::MouseButtonReleased)
        {
            mouse_pressed = false;
        }

        if (event.type == sf::Event::MouseWheelScrolled)
        {
            m_distance += static_cast<float> (event.mouseWheelScroll.delta) * -1.0f;
            m_distance = m_distance < 1.0f ? 1.0f : m_distance;

            m_matrix = glm::lookAt(position(), m_center, m_up);
        }

        return false;
    }

    [[nodiscard]] glm::mat4 matrix () const
    {
        return m_matrix;
    }
};

class Projection
{
private:

    float m_fovy;
    float m_z_near;
    float m_z_far;
    glm::mat4 m_matrix;

public:

    Projection (float width, float height)
            : m_fovy(glm::radians(110.0f))
            , m_z_near(0.01f)
            , m_z_far(10000.0f)
            , m_matrix(glm::perspective(m_fovy, width / height, m_z_near, m_z_far))
    {
    }

    bool update (sf::Event event)
    {
        if (event.type == sf::Event::Resized)
        {
            glViewport(0, 0, event.size.width, event.size.height);

            const auto width = static_cast<float> (event.size.width);
            const auto height = static_cast<float> (event.size.height);
            const auto aspect = width / height;

            m_matrix = glm::perspective(m_fovy, aspect, m_z_near, m_z_far);

            return true;
        }
        return false;
    }

    [[nodiscard]] glm::mat4 matrix () const
    {
        return m_matrix;
    }
};
