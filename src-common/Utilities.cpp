#include "Utilities.hpp"
#include <glm/gtc/random.hpp>

glm::vec3 random_vec3(float min, float max)
{
    return glm::vec3(glm::linearRand(min, max), glm::linearRand(min, max), glm::linearRand(min, max));
}
