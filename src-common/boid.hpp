#include <vector>
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
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

    void avoid_walls(const glm::vec3& min_window_size, const glm::vec3& max_window_size, const float& wall_distance);

    void display_boid(p6::Context& context);

    void update_boid(const float& size, const float& speed);

    void separate(const std::vector<Boid>& boids, const float& separation_distance, const float& separation_strength);

    void align(const std::vector<Boid>& boids, const float& alignment_distance, const float& alignment_strength);

    void cohesion(const std::vector<Boid>& boids, const float& cohesion_distance, const float& cohesion_strength);

    glm::vec3 get_position() const;

    float get_size() const;
};