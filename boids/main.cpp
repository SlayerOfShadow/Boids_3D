#include <p6/p6.h>
#include "3DParser.hpp"
#include "Boid.hpp"
#include "FreeflyCamera.hpp"
#include "GLFW/glfw3.h"
#include "Setup.hpp"
#include "glm/gtc/random.hpp"
#include "glm/gtc/type_ptr.hpp"

struct BoidProgram {
    p6::Shader m_Program;

    GLuint uMVPMatrix;
    GLuint uMVMatrix;
    GLuint uNormalMatrix;
    GLuint uTexture;

    BoidProgram()
        : m_Program{p6::load_shader("shaders/3D.vs.glsl", "shaders/tex3D.fs.glsl")}
    {
        uMVPMatrix    = glGetUniformLocation(m_Program.id(), "uMVPMatrix");
        uMVMatrix     = glGetUniformLocation(m_Program.id(), "uMVMatrix");
        uNormalMatrix = glGetUniformLocation(m_Program.id(), "uNormalMatrix");
        uTexture      = glGetUniformLocation(m_Program.id(), "uTexture");
    }
};

glm::vec3 random_vec3(float min, float max)
{
    return glm::vec3(glm::linearRand(min, max), glm::linearRand(min, max), glm::linearRand(min, max));
}

int main()
{
    srand(time(0));

    auto ctx = p6::Context{{1280, 720, "Boids 3D"}};
    ctx.maximize_window();

    BoidProgram boidProgram{};

    img::Image earth_image = p6::load_image_buffer("./assets/textures/EarthMap.jpg");
    GLuint     earth_texture;

    img::Image background_image = p6::load_image_buffer("./assets/textures/background.jpg");
    GLuint     background_texture;

    glGenTextures(1, &earth_texture);
    glBindTexture(GL_TEXTURE_2D, earth_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, earth_image.width(), earth_image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, earth_image.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &background_texture);
    glBindTexture(GL_TEXTURE_2D, background_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, background_image.width(), background_image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, background_image.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    ////////// VBOS & VAOS //////////

    GLuint                           vbo1;
    std::vector<glimac::ShapeVertex> boid_LD = loadObjFile("./assets/models/cubeLD.obj");
    create_vbo(vbo1, boid_LD);

    GLuint                           vbo2;
    std::vector<glimac::ShapeVertex> boid_HD = loadObjFile("./assets/models/cubeHD.obj");
    create_vbo(vbo2, boid_HD);

    GLuint                           vbo3;
    std::vector<glimac::ShapeVertex> background = loadObjFile("./assets/models/background.obj");
    create_vbo(vbo3, background);

    GLuint vao1;
    create_vao(vao1, vbo1);

    GLuint vao2;
    create_vao(vao2, vbo2);

    /////////////////////////////////

    glEnable(GL_DEPTH_TEST);

    glm::mat4              ProjMatrix, MVMatrix, NormalMatrix;
    std::vector<glm::mat4> MV_transformations;
    FreeflyCamera          f_camera;

    ProjMatrix   = glm::perspective(glm::radians(70.f), ctx.aspect_ratio(), 0.1f, 100.0f);
    NormalMatrix = glm::transpose(glm::inverse(MVMatrix));

    MV_transformations.push_back(MVMatrix);

    // Parameters
    std::vector<Boid> boids;
    size_t            nb_boids              = 30;
    float             boid_size             = 0.25f;
    float             boid_speed            = 0.05f;
    float             wall_distance         = 3.0f;
    float             avoid_wall_smoothness = 0.01f;

    float separation_distance = 1.0f;
    float alignement_distance = 1.0f;
    float cohesion_distance   = 1.0f;
    float separation_strength = 0.55f;
    float alignement_strength = 0.25f;
    float cohesion_strength   = 0.385f;

    // Display parameters
    bool lowQuality = false;

    ctx.imgui = [&]() {
        ImGui::Begin("Parameters");
        ImGui::SliderFloat("Size", &boid_size, 0.1f, 1.0f);
        ImGui::SliderFloat("Speed", &boid_speed, 0.01f, 0.1f);
        ImGui::SliderFloat("Separation", &separation_strength, 0.0f, 1.0f);
        ImGui::SliderFloat("Alignement", &alignement_strength, 0.0f, 1.0f);
        ImGui::SliderFloat("Cohesion", &cohesion_strength, 0.0f, 1.0f);
        bool changeQuality = ImGui::Button("LD/HD");
        ImGui::End();

        if (changeQuality)
        {
            lowQuality = !lowQuality;
        }
    };

    for (size_t i = 0; i < nb_boids; i++)
    {
        boids.push_back(Boid(boid_size, boid_speed, random_vec3(-3.0f, 3.0f), normalize(random_vec3(-1.0f, 1.0f))));
        MV_transformations.push_back(MVMatrix);
    }

    // Application loop :
    ctx.update = [&]() {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        MV_transformations[0] = f_camera.getViewMatrix();

        boidProgram.m_Program.use();
        glUniform1i(boidProgram.uTexture, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, earth_texture);
        (lowQuality ? glBindVertexArray(vao1) : glBindVertexArray(vao2));

        for (size_t i = 0; i < nb_boids; i++)
        {
            boids[i].move_boid();
            boids[i].avoid_walls(glm::vec3(-wall_distance), glm::vec3(wall_distance), avoid_wall_smoothness);
            boids[i].update_boid(boid_size, boid_speed);
            boids[i].separate(boids, separation_distance, separation_strength);
            boids[i].align(boids, alignement_distance, alignement_strength);
            boids[i].cohesion(boids, cohesion_distance, cohesion_strength);

            glm::mat4 R = glm::lookAt(glm::vec3(0, 0, 0), boids[i].get_direction(), glm::vec3(0, 1, 0));

            MV_transformations[i + 1] = glm::translate(MV_transformations[0], boids[i].get_position());
            MV_transformations[i + 1] *= R;
            MV_transformations[i + 1] = glm::scale(MV_transformations[i + 1], glm::vec3(boid_size));

            glUniformMatrix4fv(boidProgram.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MV_transformations[i + 1]));
            glUniformMatrix4fv(boidProgram.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MV_transformations[i + 1]));
            glUniformMatrix4fv(boidProgram.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

            (lowQuality ? glDrawArrays(GL_TRIANGLES, 0, boid_LD.size()) : glDrawArrays(GL_TRIANGLES, 0, boid_HD.size()));
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
    glDeleteBuffers(1, &vbo1);
    glDeleteBuffers(1, &vbo2);
    glDeleteVertexArrays(1, &vao1);
    glDeleteVertexArrays(1, &vao2);
    glDeleteTextures(1, &earth_texture);
    glDeleteTextures(1, &background_texture);
}