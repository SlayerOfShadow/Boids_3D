#pragma once

#include "p6/p6.h"

class Boid {
private:
    float     m_size;
    float     m_speed;
    glm::vec3 m_position;
    glm::vec3 m_direction;

public:
    // Constructors
    Boid() = default;

    ~Boid() = default;

    Boid(const float& size, const float& speed, const glm::vec3& position, const glm::vec3& direction)
        : m_size(size), m_speed(speed), m_position(position), m_direction(direction) {}

    // Methods
    void move_boid();

    void Boid::avoid_walls(const glm::vec3& min, const glm::vec3& max, const float& smooth, const std::vector<glm::vec3>& asteroidPositions, const float& avoidanceRadius, const float& avoidanceForce);

    void update_boid(const float& size, const float& speed);

    void separate(const std::vector<Boid>& boids, const float& separation_distance, const float& separation_strength);

    void align(const std::vector<Boid>& boids, const float& alignment_distance, const float& alignment_strength);

    void cohesion(const std::vector<Boid>& boids, const float& cohesion_distance, const float& cohesion_strength);

    glm::vec3 get_position() const;

    glm::vec3 get_direction() const;
};