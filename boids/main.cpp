#include <p6/p6.h>
#include <vcruntime.h>
#include <vector>
#include "../src-common/glimac/common.hpp"
#include "../src-common/glimac/sphere_vertices.hpp"
#include "glimac/common.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/random.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "img/src/Image.h"

int main()
{
    auto ctx = p6::Context{{1280, 720, "Boids"}};
    ctx.maximize_window();

    const p6::Shader shader = p6::load_shader(
        "shaders/3D.vs.glsl",
        "shaders/tex3D.fs.glsl"
    );
    shader.use();

    img::Image earth_image = p6::load_image_buffer("assets/textures/EarthMap.jpg");
    img::Image moon_image  = p6::load_image_buffer("assets/textures/MoonMap.jpg");
    GLuint     earth_texture;
    GLuint     moon_texture;

    glGenTextures(1, &earth_texture);
    glBindTexture(GL_TEXTURE_2D, earth_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, earth_image.width(), earth_image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, earth_image.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &moon_texture);
    glBindTexture(GL_TEXTURE_2D, moon_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, moon_image.width(), moon_image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, moon_image.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint uMVPMatrixLocation    = glGetUniformLocation(shader.id(), "uMVPMatrix");
    GLuint uMVMatrixLocation     = glGetUniformLocation(shader.id(), "uMVMatrix");
    GLuint uNormalMatrixLocation = glGetUniformLocation(shader.id(), "uNormalMatrix");
    GLuint uTexture              = glGetUniformLocation(shader.id(), "uTexture");

    GLuint vbo;
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    size_t                           nb_sphere = 32;
    std::vector<glimac::ShapeVertex> sphere    = glimac::sphere_vertices(1.f, 32, 16);

    glBufferData(GL_ARRAY_BUFFER, sphere.size() * sizeof(glimac::ShapeVertex), sphere.data(), GL_STATIC_DRAW);

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

    std::vector<glm::vec3> random_axes;
    std::vector<glm::vec3> start_positions;
    for (size_t i = 0; i < nb_sphere; i++)
    {
        random_axes.push_back(glm::sphericalRand(60.0f));
        start_positions.push_back(glm::sphericalRand(2.0f));
    }

    // Application loop :
    ctx.update = [&]() {
        shader.use();
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // EARTH
        glBindTexture(GL_TEXTURE_2D, earth_texture);
        glUniform1i(uTexture, 0);

        glm::mat4 ProjMatrix   = glm::perspective(glm::radians(70.f), ctx.aspect_ratio(), 0.1f, 100.0f);
        glm::mat4 MVMatrix     = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
        glm::mat4 NormalMatrix = glm::transpose(glm::inverse(MVMatrix));

        glUniformMatrix4fv(uMVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));
        glUniformMatrix4fv(uMVMatrixLocation, 1, GL_FALSE, glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(uNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, sphere.size());

        glBindTexture(GL_TEXTURE_2D, 0);

        glBindTexture(GL_TEXTURE_2D, moon_texture);
        glUniform1i(uTexture, 0);

        // MOONS
        for (size_t i = 1; i < nb_sphere; i++)
        {
            MVMatrix = glm::translate(glm::mat4{1.f}, {0.f, 0.f, -5.f});                                  // Translation
            MVMatrix = glm::rotate(MVMatrix, ctx.time(), glm::cross(random_axes[i], start_positions[i])); // Translation * Rotation
            MVMatrix = glm::translate(MVMatrix, start_positions[i]);                                      // Translation * Rotation * Translation
            MVMatrix = glm::scale(MVMatrix, glm::vec3{0.2f});                                             // Translation * Rotation * Translation * Scale

            glUniformMatrix4fv(uMVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));
            glUniformMatrix4fv(uMVMatrixLocation, 1, GL_FALSE, glm::value_ptr(MVMatrix));
            glUniformMatrix4fv(uNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
            glDrawArrays(GL_TRIANGLES, 0, sphere.size());
        }

        glBindTexture(GL_TEXTURE_2D, 0);

        glBindVertexArray(0);
    };

    // Start the update loop
    ctx.start();

    // Cleanup once the window is closed
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteTextures(1, &earth_texture);
}