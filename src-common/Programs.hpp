#pragma once

#include <p6/p6.h>

struct OneTextureProgram {
    p6::Shader m_Program;

    GLuint uMVPMatrix;
    GLuint uMVMatrix;
    GLuint uNormalMatrix;
    GLuint uTexture;

    OneTextureProgram()
        : m_Program{p6::load_shader("shaders/3D.vs.glsl", "shaders/tex3D.fs.glsl")}
    {
        uMVPMatrix    = glGetUniformLocation(m_Program.id(), "uMVPMatrix");
        uMVMatrix     = glGetUniformLocation(m_Program.id(), "uMVMatrix");
        uNormalMatrix = glGetUniformLocation(m_Program.id(), "uNormalMatrix");
        uTexture      = glGetUniformLocation(m_Program.id(), "uTexture");
    }
};

struct OneTextureLightProgram {
    p6::Shader m_Program;

    GLuint uMVPMatrix;
    GLuint uMVMatrix;
    GLuint uNormalMatrix;
    GLuint uTexture;

    GLint m_uKd;
    GLint m_uKs;
    GLint m_uShininess;

    GLint m_uPointLightPos_vs;
    GLint m_uPointLightIntensity;
    GLint m_uPointLightColor;

    OneTextureLightProgram()
        : m_Program{p6::load_shader("shaders/3D.vs.glsl", "shaders/tex3DLight.fs.glsl")}
    {
        uMVPMatrix             = glGetUniformLocation(m_Program.id(), "uMVPMatrix");
        uMVMatrix              = glGetUniformLocation(m_Program.id(), "uMVMatrix");
        uNormalMatrix          = glGetUniformLocation(m_Program.id(), "uNormalMatrix");
        uTexture               = glGetUniformLocation(m_Program.id(), "uTexture");
        m_uKd                  = glGetUniformLocation(m_Program.id(), "uKd");
        m_uKs                  = glGetUniformLocation(m_Program.id(), "uKs");
        m_uShininess           = glGetUniformLocation(m_Program.id(), "uShininess");
        m_uPointLightPos_vs    = glGetUniformLocation(m_Program.id(), "uPointLightPos_vs");
        m_uPointLightColor     = glGetUniformLocation(m_Program.id(), "uPointLightColor");
        m_uPointLightIntensity = glGetUniformLocation(m_Program.id(), "uPointLightIntensity");
    }
};