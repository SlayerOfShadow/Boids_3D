#pragma once

#include <p6/p6.h>
#include "glimac/common.hpp"

void create_texture(img::Image& image, GLuint& texture);

void create_vbo(GLuint& vbo, std::vector<glimac::ShapeVertex>& shape);

void create_vao(GLuint& vao, GLuint& vbo);

void draw_object();