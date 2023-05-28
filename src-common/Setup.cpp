#include "Setup.hpp"
#include <vector>
#include "glimac/common.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/type_ptr.hpp"

void create_texture(img::Image& image, GLuint& texture)
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void create_vbo(GLuint& vbo, std::vector<glimac::ShapeVertex>& shape)
{
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, shape.size() * sizeof(glimac::ShapeVertex), shape.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void create_vao(GLuint& vao, GLuint& vbo)
{
    static constexpr GLuint VERTEX_ATTR_POSITION  = 0;
    static constexpr GLuint VERTEX_ATTR_NORMAL    = 1;
    static constexpr GLuint VERTEX_ATTR_TEXCOORDS = 2;

    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);

    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(VERTEX_ATTR_NORMAL);
    glEnableVertexAttribArray(VERTEX_ATTR_TEXCOORDS);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexAttribPointer(VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)offsetof(glimac::ShapeVertex, position));
    glVertexAttribPointer(VERTEX_ATTR_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)offsetof(glimac::ShapeVertex, normal));
    glVertexAttribPointer(VERTEX_ATTR_TEXCOORDS, 2, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)offsetof(glimac::ShapeVertex, texCoords));

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

void drawOneTexture(OneTextureProgram& program, GLuint texture, std::vector<glimac::ShapeVertex> shape, GLuint vao, glm::mat4 mvpMatrix, glm::mat4 mvMatrix, glm::mat4 normalMatrix)
{
    program.m_Program.use();
    glUniform1i(program.uTexture, 0);
    glActiveTexture(GL_TEXTURE0);

    glUniformMatrix4fv(program.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
    glUniformMatrix4fv(program.uMVMatrix, 1, GL_FALSE, glm::value_ptr(mvMatrix));
    glUniformMatrix4fv(program.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, shape.size());
}

void drawOneTextureLight(OneTextureLightProgram& program, GLuint texture, std::vector<glimac::ShapeVertex> lqShape, std::vector<glimac::ShapeVertex> hqShape, GLuint lqVao, GLuint hqVao, glm::mat4 mvpMatrix, glm::mat4 mvMatrix, glm::mat4 normalMatrix, bool lowQuality, glm::vec3 kd, glm::vec3 ks, float shininess, DirectionalLight directionalLight, PointLight pointLight, bool dirLight, FreeflyCamera fCamera)
{
    program.m_Program.use();
    glUniform1i(program.uTexture, 0);
    glActiveTexture(GL_TEXTURE0);

    glUniform3fv(program.m_uPointLightPos_vs, 1, glm::value_ptr(glm::vec3(fCamera.getViewMatrix() * glm::vec4(pointLight.pointLightPos, 1))));
    glUniform3fv(program.m_uPointLightColor, 1, glm::value_ptr(pointLight.pointLightColor));
    glUniform1fv(program.m_uPointLightIntensity, 1, &pointLight.pointLightIntensity);

    directionalLight.dirLightDir = fCamera.getFront();
    glUniform3fv(program.m_uDirLightDir_vs, 1, glm::value_ptr(glm::vec3(fCamera.getViewMatrix() * glm::vec4(directionalLight.dirLightDir, 0.0f))));
    (dirLight ? directionalLight.dirLightColor = glm::vec3(0.5f) : directionalLight.dirLightColor = glm::vec3(0.0f));
    glUniform3fv(program.m_uDirLightColor, 1, glm::value_ptr(directionalLight.dirLightColor));

    glUniform3fv(program.m_uKd, 1, glm::value_ptr(kd));
    glUniform3fv(program.m_uKs, 1, glm::value_ptr(ks));
    glUniform1f(program.m_uShininess, shininess);

    glUniformMatrix4fv(program.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
    glUniformMatrix4fv(program.uMVMatrix, 1, GL_FALSE, glm::value_ptr(mvMatrix));
    glUniformMatrix4fv(program.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    glBindTexture(GL_TEXTURE_2D, texture);
    (lowQuality ? glBindVertexArray(lqVao) : glBindVertexArray(hqVao));
    (lowQuality ? glDrawArrays(GL_TRIANGLES, 0, lqShape.size()) : glDrawArrays(GL_TRIANGLES, 0, hqShape.size()));
}