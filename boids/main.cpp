#include <p6/p6.h>
#include <stdio.h>
#include <vcruntime.h>
#include <vector>
#include "3DParser.hpp"
#include "Boid.hpp"
#include "FreeflyCamera.hpp"
#include "GLFW/glfw3.h"
#include "Programs.hpp"
#include "Setup.hpp"
#include "Utilities.hpp"
#include "glm/gtc/random.hpp"
#include "glm/gtc/type_ptr.hpp"

int main()
{
    srand(time(0));

    auto ctx = p6::Context{{1280, 720, "Boids 3D"}};
    ctx.maximize_window();

    ////////// PROGRAMS //////////

    OneTextureProgram boid_program{};
    OneTextureProgram background_program{};

    ////////// TEXTURES //////////

    size_t                  nb_textures = 2;
    std::vector<img::Image> images;
    std::vector<GLuint>     textures(nb_textures);

    images.push_back(p6::load_image_buffer("./assets/textures/EarthMap.jpg"));
    images.push_back(p6::load_image_buffer("./assets/textures/background.jpg"));

    for (size_t i = 0; i < nb_textures; i++)
    {
        create_texture(images[i], textures[i]);
    }

    ////////// VBOS & VAOS //////////

    size_t                                        nb_objects = 3;
    std::vector<GLuint>                           vbos(nb_objects);
    std::vector<GLuint>                           vaos(nb_objects);
    std::vector<std::vector<glimac::ShapeVertex>> shapes;

    shapes.push_back(loadObjFile("./assets/models/cubeLD.obj"));
    shapes.push_back(loadObjFile("./assets/models/cubeHD.obj"));
    shapes.push_back(loadObjFile("./assets/models/background.obj"));

    for (size_t i = 0; i < nb_objects; i++)
    {
        create_vbo(vbos[i], shapes[i]);
        create_vao(vaos[i], vbos[i]);
    }

    ////////// PARAMETERS //////////

    std::vector<Boid> boids;
    size_t            nb_boids              = 200;
    float             boid_size             = 0.75f;
    float             boid_speed            = 0.1f;
    float             wall_distance         = 85.0f;
    float             avoid_wall_smoothness = 0.01f;

    float separation_distance = 5.0f;
    float alignement_distance = 5.0f;
    float cohesion_distance   = 5.0f;
    float separation_strength = 1.0f;
    float alignement_strength = 0.25f;
    float cohesion_strength   = 0.125f;
    bool  low_quality         = false;

    ctx.imgui = [&]() {
        ImGui::Begin("Parameters");
        ImGui::SliderFloat("Size", &boid_size, 0.1f, 1.0f);
        ImGui::SliderFloat("Speed", &boid_speed, 0.05f, 0.2f);
        ImGui::SliderFloat("Separation", &separation_strength, 0.0f, 1.0f);
        ImGui::SliderFloat("Alignement", &alignement_strength, 0.0f, 1.0f);
        ImGui::SliderFloat("Cohesion", &cohesion_strength, 0.0f, 1.0f);
        ImGui::SliderFloat("Separation distance", &separation_distance, 0.0f, 1.0f);
        ImGui::SliderFloat("Alignement distance", &alignement_distance, 0.0f, 1.0f);
        ImGui::SliderFloat("Cohesion distance", &cohesion_distance, 0.0f, 1.0f);
        bool change_quality = ImGui::Button("LD/HD");
        ImGui::End();

        if (change_quality)
        {
            low_quality = !low_quality;
        }
    };

    ////////// SETUP & CAMERA //////////

    glm::mat4              ProjMatrix, MVMatrix, NormalMatrix;
    std::vector<glm::mat4> MV_transformations;
    FreeflyCamera          f_camera;

    ProjMatrix   = glm::perspective(glm::radians(70.f), ctx.aspect_ratio(), 0.1f, 200.0f);
    NormalMatrix = glm::transpose(glm::inverse(MVMatrix));

    MV_transformations.push_back(MVMatrix);

    for (size_t i = 0; i < nb_boids; i++)
    {
        boids.push_back(Boid(boid_size, boid_speed, random_vec3(-wall_distance, wall_distance), normalize(random_vec3(-1.0f, 1.0f))));
        MV_transformations.push_back(MVMatrix);
    }

    glEnable(GL_DEPTH_TEST);

    ////////// APPLICATION LOOP //////////

    ctx.update = [&]() {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        MV_transformations[0] = f_camera.getViewMatrix();

        ////////// BACKGROUND //////////

        background_program.m_Program.use();

        glUniform1i(background_program.uTexture, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[1]);

        glUniformMatrix4fv(background_program.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MV_transformations[0]));
        glUniformMatrix4fv(background_program.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MV_transformations[0]));
        glUniformMatrix4fv(background_program.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

        glBindVertexArray(vaos[2]);
        glDrawArrays(GL_TRIANGLES, 0, shapes[2].size());

        ////////// BOIDS //////////

        boid_program.m_Program.use();

        glUniform1i(boid_program.uTexture, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[0]);

        (low_quality ? glBindVertexArray(vaos[0]) : glBindVertexArray(vaos[1]));

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

            glUniformMatrix4fv(boid_program.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MV_transformations[i + 1]));
            glUniformMatrix4fv(boid_program.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MV_transformations[i + 1]));
            glUniformMatrix4fv(boid_program.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

            (low_quality ? glDrawArrays(GL_TRIANGLES, 0, shapes[0].size()) : glDrawArrays(GL_TRIANGLES, 0, shapes[1].size()));
        }

        ////////// INPUTS //////////

        if (ctx.key_is_pressed(GLFW_KEY_A))
        {
            f_camera.moveLeft(0.1f, wall_distance);
        }

        if (ctx.key_is_pressed(GLFW_KEY_D))
        {
            f_camera.moveLeft(-0.1f, wall_distance);
        }

        if (ctx.key_is_pressed(GLFW_KEY_S))
        {
            f_camera.moveFront(-0.1f, wall_distance);
        }

        if (ctx.key_is_pressed(GLFW_KEY_W))
        {
            f_camera.moveFront(0.1f, wall_distance);
        }
    };

    ctx.mouse_dragged = [&f_camera](p6::MouseDrag mouse_drag) {
        f_camera.rotateLeft(mouse_drag.delta.x * 100);
        f_camera.rotateUp(-mouse_drag.delta.y * 100);
    };

    ctx.start();

    ////////// CLEAR //////////

    for (size_t i = 0; i < nb_objects; i++)
    {
        glDeleteBuffers(1, &vbos[i]);
        glDeleteVertexArrays(1, &vaos[i]);
    }

    for (size_t i = 0; i < nb_textures; i++)
    {
        glDeleteTextures(1, &textures[i]);
    }
}