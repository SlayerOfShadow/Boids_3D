#include "glm/ext/matrix_transform.hpp"

class TrackballCamera {
private:
    float m_Distance;
    float m_AngleX;
    float m_AngleY;

public:
    TrackballCamera()
        : m_Distance(0.f), m_AngleX(0.f), m_AngleY(0.f){};
    void      moveFront(float delta);
    void      rotateLeft(float degrees);
    void      rotateUp(float degrees);
    glm::mat4 getViewMatrix() const;
};