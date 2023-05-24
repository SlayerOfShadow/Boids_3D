#include "FreeflyCamera.hpp"
#include "glm/fwd.hpp"

void FreeflyCamera::computeDirectionVectors()
{
    this->m_FrontVector = glm::vec3(glm::cos(this->m_Theta) * glm::sin(this->m_Phi), glm::sin(this->m_Theta), glm::cos(this->m_Theta) * glm::cos(this->m_Phi));

    this->m_LeftVector = glm::vec3(glm::sin(this->m_Phi + glm::half_pi<float>()), 0, glm::cos(this->m_Phi + glm::half_pi<float>()));

    this->m_UpVector = glm::cross(this->m_FrontVector, this->m_LeftVector);
};

void FreeflyCamera::moveLeft(float t, float wall_distance)
{
    glm::vec3 newPosition = m_Position + t * m_LeftVector;
    if (newPosition.x >= -wall_distance && newPosition.x <= wall_distance && newPosition.y >= -wall_distance && newPosition.y <= wall_distance && newPosition.z >= -wall_distance && newPosition.z <= wall_distance)
    {
        m_Position = newPosition;
        computeDirectionVectors();
    }
}

void FreeflyCamera::moveFront(float t, float wall_distance)
{
    glm::vec3 newPosition = m_Position + t * m_FrontVector;
    if (newPosition.x >= -wall_distance && newPosition.x <= wall_distance && newPosition.y >= -wall_distance && newPosition.y <= wall_distance && newPosition.z >= -wall_distance && newPosition.z <= wall_distance)
    {
        m_Position = newPosition;
        computeDirectionVectors();
    }
}

void FreeflyCamera::rotateLeft(float degrees)
{
    this->m_Phi += -glm::radians(degrees);
    this->computeDirectionVectors();
};

void FreeflyCamera::rotateUp(float degrees)
{
    this->m_Theta += -glm::radians(degrees);
    this->computeDirectionVectors();
};

glm::mat4 FreeflyCamera::getViewMatrix() const
{
    return glm::lookAt(this->m_Position, glm::vec3(this->m_Position + this->m_FrontVector), this->m_UpVector);
};

glm::vec3 FreeflyCamera::getPosition() const
{
    return m_Position;
}

glm::vec3 FreeflyCamera::getFront() const
{
    return m_FrontVector;
}