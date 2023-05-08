#include <p6/p6.h>
#include <stdio.h>
#include <vcruntime.h>
#include <vector>
#include "../src-common/boid.hpp"
#include "../src-common/glimac/common.hpp"
#include "../src-common/glimac/sphere_vertices.hpp"
#include "FreeflyCamera.hpp"
#include "GLFW/glfw3.h"
#include "TrackballCamera.hpp"
#include "glimac/common.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/random.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "img/src/Image.h"

struct BoidProgram {
    p6::Shader m_Program;

    GLuint uMVPMatrix;
    GLuint uMVMatrix;
    GLuint uNormalMatrix;
    GLuint uTexture1;
    GLuint uTexture2;

    BoidProgram()
        : m_Program{p6::load_shader("shaders/3D.vs.glsl", "shaders/multiTex3D.fs.glsl")}
    {
        uMVPMatrix    = glGetUniformLocation(m_Program.id(), "uMVPMatrix");
        uMVMatrix     = glGetUniformLocation(m_Program.id(), "uMVMatrix");
        uNormalMatrix = glGetUniformLocation(m_Program.id(), "uNormalMatrix");
        uTexture1     = glGetUniformLocation(m_Program.id(), "uTexture1");
        uTexture2     = glGetUniformLocation(m_Program.id(), "uTexture2");
    }
};

glm::vec3 random_vec3(float min, float max)
{
    static std::random_device                    rd;
    static std::mt19937                          gen(rd());
    static std::uniform_real_distribution<float> dist(min, max);

    return glm::vec3(dist(gen), dist(gen), 0);
}

int main()
{
    auto ctx = p6::Context{{1280, 720, "Boids 3D"}};
    ctx.maximize_window();

    BoidProgram boidProgram{};

    img::Image earth_image = p6::load_image_buffer("assets/textures/EarthMap.jpg");
    img::Image cloud_image = p6::load_image_buffer("assets/textures/CloudMap.jpg");
    GLuint     earth_texture;
    GLuint     cloud_texture;

    glGenTextures(1, &earth_texture);
    glBindTexture(GL_TEXTURE_2D, earth_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, earth_image.width(), earth_image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, earth_image.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &cloud_texture);
    glBindTexture(GL_TEXTURE_2D, cloud_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cloud_image.width(), cloud_image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, cloud_image.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint vbo;
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    std::vector<Boid>                boids;
    size_t                           nb_boids      = 5;
    float                            boid_size     = 0.5f;
    float                            boid_speed    = 1.0f;
    float                            wall_distance = 35.0f;
    std::vector<glimac::ShapeVertex> boid_shape    = glimac::sphere_vertices(1.f, 32, 16);

    glBufferData(GL_ARRAY_BUFFER, boid_shape.size() * sizeof(glimac::ShapeVertex), boid_shape.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint vao;
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);

    static constexpr GLuint VERTEX_ATTR_POSITION  = 0;
    static constexpr GLuint VERTEX_ATTR_NORMAL    = 1;
    static constexpr GLuint VERTEX_ATTR_TEXCOORDS = 2;
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(VERTEX_ATTR_NORMAL);
    glEnableVertexAttribArray(VERTEX_ATTR_TEXCOORDS);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)offsetof(glimac::ShapeVertex, position));
    glVertexAttribPointer(VERTEX_ATTR_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)offsetof(glimac::ShapeVertex, normal));
    glVertexAttribPointer(VERTEX_ATTR_TEXCOORDS, 2, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)offsetof(glimac::ShapeVertex, texCoords));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);

    glm::mat4              ProjMatrix, MVMatrix, NormalMatrix;
    std::vector<glm::mat4> MV_transformations;
    FreeflyCamera          f_camera;

    ProjMatrix   = glm::perspective(glm::radians(70.f), ctx.aspect_ratio(), 0.1f, 100.0f);
    NormalMatrix = glm::transpose(glm::inverse(MVMatrix));

    MV_transformations.push_back(MVMatrix);

    for (size_t i = 0; i < nb_boids; i++)
    {
        boids.push_back(Boid(boid_size, boid_speed, random_vec3(-3.0f, 3.0f), glm::vec3(0, 0, 0)));
        MV_transformations.push_back(MVMatrix);
    }

    // Application loop :
    ctx.update = [&]() {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        MV_transformations[0] = f_camera.getViewMatrix();

        boidProgram.m_Program.use();

        glUniform1i(boidProgram.uTexture1, 0);
        glUniform1i(boidProgram.uTexture2, 1);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, earth_texture); // la texture earth_texture est bindée sur l'unité GL_TEXTURE0
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, cloud_texture); // la texture cloud_texture est bindée sur l'unité GL_TEXTURE1

        glBindVertexArray(vao);

        for (size_t i = 0; i < nb_boids; i++)
        {
            MV_transformations[i + 1] = glm::translate(MV_transformations[0], boids[i].get_position());
            MV_transformations[i + 1] = glm::scale(MV_transformations[i + 1], glm::vec3(boid_size));

            glUniformMatrix4fv(boidProgram.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MV_transformations[i + 1]));
            glUniformMatrix4fv(boidProgram.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MV_transformations[i + 1]));
            glUniformMatrix4fv(boidProgram.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

            glDrawArrays(GL_TRIANGLES, 0, boid_shape.size());
        }

        // INPUTS
        if (ctx.key_is_pressed(GLFW_KEY_A))
        {
            f_camera.moveLeft(0.1f);
        }

        if (ctx.key_is_pressed(GLFW_KEY_D))
        {
            f_camera.moveLeft(-0.1f);
        }

        if (ctx.key_is_pressed(GLFW_KEY_S))
        {
            f_camera.moveFront(-0.1f);
        }

        if (ctx.key_is_pressed(GLFW_KEY_W))
        {
            f_camera.moveFront(0.1f);
        }
    };

    ctx.mouse_dragged = [&f_camera](p6::MouseDrag mouse_drag) {
        f_camera.rotateLeft(mouse_drag.delta.x * 100);
        f_camera.rotateUp(-mouse_drag.delta.y * 100);
    };

    // Start the update loop
    ctx.start();

    // Cleanup once the window is closed
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteTextures(1, &earth_texture);
}