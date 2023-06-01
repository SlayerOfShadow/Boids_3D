#include "Boid.hpp"

//  Methods
void Boid::move_boid()
{
    m_position += m_direction * m_speed;
}

void Boid::avoid_walls(const glm::vec3& min, const glm::vec3& max, const float& smooth, const std::vector<glm::vec3>& asteroidPositions, const float& avoidanceRadius, const float& avoidanceForce)
{
    glm::vec3 force(0.0f);

    // Avoid walls
    if (m_position.x < min.x)
    {
        force.x = (min.x - m_position.x);
    }
    else if (m_position.x > max.x)
    {
        force.x = (max.x - m_position.x);
    }
    if (m_position.y < min.y)
    {
        force.y = (min.y - m_position.y);
    }
    else if (m_position.y > max.y)
    {
        force.y = (max.y - m_position.y);
    }
    if (m_position.z < min.z)
    {
        force.z = (min.z - m_position.z);
    }
    else if (m_position.z > max.z)
    {
        force.z = (max.z - m_position.z);
    }

    // Avoid asteroids
    for (const glm::vec3& asteroidPos : asteroidPositions)
    {
        glm::vec3 toAsteroid = m_position - asteroidPos;
        float     distance   = glm::length(toAsteroid);
        if (distance < avoidanceRadius)
        {
            force += (avoidanceRadius - distance) * toAsteroid;
        }
    }

    m_direction += force * smooth;
    m_direction = glm::normalize(m_direction);
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

glm::vec3 Boid::get_direction() const
{
    return m_direction;
}