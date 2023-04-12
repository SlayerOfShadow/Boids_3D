#include "TrackballCamera.hpp"

void TrackballCamera::moveFront(float delta)
{
    m_Distance += delta;
}

void TrackballCamera::rotateLeft(float degrees)
{
    m_AngleX += degrees;
}

void TrackballCamera::rotateUp(float degrees)
{
    m_AngleY += degrees;
}

glm::mat4 TrackballCamera::getViewMatrix() const
{
    glm::mat4 MVMatrix;
    MVMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -this->m_Distance));
    MVMatrix = glm::rotate(MVMatrix, glm::radians(-this->m_AngleX), glm::vec3(1, 0, 0));
    MVMatrix = glm::rotate(MVMatrix, glm::radians(-this->m_AngleY), glm::vec3(0, 1, 0));

    return MVMatrix;
}