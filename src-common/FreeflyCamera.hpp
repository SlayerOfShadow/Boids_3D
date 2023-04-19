#include "glm/ext/matrix_transform.hpp"

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
    void      moveLeft(float t);
    void      moveFront(float t);
    void      rotateLeft(float degrees);
    void      rotateUp(float degrees);
    glm::mat4 getViewMatrix() const;
};