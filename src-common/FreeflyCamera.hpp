#pragma once

#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"

class FreeflyCamera {
private:
    glm::vec3 m_Position;
    glm::vec3 m_FrontVector;
    glm::vec3 m_LeftVector;
    glm::vec3 m_UpVector;
    float     m_Phi;
    float     m_Theta;
    void      computeDirectionVectors();

public:
    FreeflyCamera()
        : m_Position(glm::vec3(0, 0, 5)), m_Phi(glm::pi<float>()), m_Theta(0.f)
    {
        computeDirectionVectors();
    };
    void      moveLeft(float t, float wall_distance);
    void      moveFront(float t, float wall_distance);
    void      rotateLeft(float degrees);
    void      rotateUp(float degrees);
    glm::mat4 getViewMatrix() const;
    glm::vec3 getPosition() const;
    glm::vec3 getFront() const;
};