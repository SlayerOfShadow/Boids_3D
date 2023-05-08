#include "boid.hpp"
#include "glm/ext/vector_float3.hpp"

//  Methods
void Boid::move_boid()
{
    m_position += m_direction * m_speed;
}

void Boid::avoid_walls(const glm::vec3& min_window_size, const glm::vec3& max_window_size, const float& wall_distance)
{
    glm::vec3 force(0.0f);

    if (m_position.x < min_window_size.x + wall_distance)
    {
        force.x = (min_window_size.x + wall_distance - m_position.x);
    }
    else if (m_position.x > max_window_size.x - wall_distance)
    {
        force.x = (max_window_size.x - wall_distance - m_position.x);
    }
    if (m_position.y < min_window_size.y + wall_distance)
    {
        force.y = (min_window_size.y + wall_distance - m_position.y);
    }
    else if (m_position.y > max_window_size.y - wall_distance)
    {
        force.y = (max_window_size.y - wall_distance - m_position.y);
    }

    m_direction += force;
    m_direction = glm::normalize(m_direction);
}

void Boid::display_boid(p6::Context& context)
{
    context.equilateral_triangle(
        p6::Center{m_position},
        p6::Radius{m_size},
        p6::Rotation{m_direction}
    );
}

void Boid::update_boid(const float& size, const float& speed)
{
    m_size  = size;
    m_speed = speed;
}

void Boid::separate(const std::vector<Boid>& boids, const float& separation_distance, const float& separation_strength)
{
    glm::vec3 separation(0.0f);

    int count = 0;
    for (const auto& other : boids)
    {
        if (this == &other)
            continue;

        const float distance = glm::distance(m_position, other.m_position);
        if (distance < separation_distance)
        {
            separation += separation_strength * (m_position - other.m_position) / distance;
            count++;
        }
    }

    if (count > 0)
    {
        separation /= static_cast<float>(count);
        m_direction += separation;
        m_direction = glm::normalize(m_direction);
    }
}

void Boid::align(const std::vector<Boid>& boids, const float& alignment_distance, const float& alignment_strength)
{
    glm::vec3 average_direction(0.0f);

    int count = 0;
    for (const auto& other : boids)
    {
        if (this == &other)
            continue;

        const float distance = glm::distance(m_position, other.m_position);
        if (distance < alignment_distance)
        {
            average_direction += other.m_direction;
            count++;
        }
    }

    if (count > 0)
    {
        average_direction /= static_cast<float>(count);
        m_direction += alignment_strength * (average_direction - m_direction);
        m_direction = glm::normalize(m_direction);
    }
}

void Boid::cohesion(const std::vector<Boid>& boids, const float& cohesion_distance, const float& cohesion_strength)
{
    glm::vec3 average_position(0.0f);

    int count = 0;
    for (const auto& other : boids)
    {
        if (this == &other)
            continue;

        const float distance = glm::distance(m_position, other.m_position);
        if (distance < cohesion_distance)
        {
            average_position += other.m_position;
            count++;
        }
    }

    if (count > 0)
    {
        average_position /= static_cast<float>(count);
        const glm::vec3 direction_to_target = average_position - m_position;
        m_direction += cohesion_strength * direction_to_target;
        m_direction = glm::normalize(m_direction);
    }
}

glm::vec3 Boid::get_position() const
{
    return m_position;
}

float Boid::get_size() const
{
    return m_size;
}