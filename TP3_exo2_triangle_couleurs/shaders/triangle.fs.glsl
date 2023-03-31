#version 330

in vec3 vColor;

out vec4 fFragColor;

void main()
{
    fFragColor = vec4(vColor, 1.);
}