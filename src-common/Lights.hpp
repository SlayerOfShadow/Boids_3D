#pragma once

#include <p6/p6.h>
#include "glm/fwd.hpp"

struct PointLight {
    glm::vec3 pointLightPos;
    glm::vec3 pointLightColor;
    float     pointLightIntensity;

    PointLight(glm::vec3 pos, glm::vec3 col, float intensity)
    {
        pointLightPos       = pos;
        pointLightColor     = col;
        pointLightIntensity = intensity;
    }
};

struct DirectionalLight {
    glm::vec3 dirLightDir;
    glm::vec3 dirLightColor;

    DirectionalLight(glm::vec3 dir, glm::vec3 col)
    {
        dirLightDir   = dir;
        dirLightColor = col;
    }
};