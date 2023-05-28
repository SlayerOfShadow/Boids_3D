#pragma once

#include <p6/p6.h>
#include "FreeflyCamera.hpp"
#include "Lights.hpp"
#include "Programs.hpp"
#include "glimac/common.hpp"

void create_texture(img::Image& image, GLuint& texture);

void create_vbo(GLuint& vbo, std::vector<glimac::ShapeVertex>& shape);

void create_vao(GLuint& vao, GLuint& vbo);

void drawOneTexture(OneTextureProgram& program, GLuint texture, std::vector<glimac::ShapeVertex> shape, GLuint vao, glm::mat4 mvpMatrix, glm::mat4 mvMatrix, glm::mat4 normalMatrix);

void drawOneTextureLight(OneTextureLightProgram& program, GLuint texture, std::vector<glimac::ShapeVertex> lqShape, std::vector<glimac::ShapeVertex> hqShape, GLuint lqVao, GLuint hqVao, glm::mat4 mvpMatrix, glm::mat4 mvMatrix, glm::mat4 normalMatrix, bool lowQuality, glm::vec3 kd, glm::vec3 ks, float shininess, DirectionalLight directionalLight, PointLight pointLight, bool dirLight, FreeflyCamera fCamera);