#version 330

in vec2 vTexCoords;

uniform sampler2D uTexture;

out vec4 fFragColor;

void main() {
    fFragColor = vec4(vec3(texture(uTexture, vTexCoords)), 1);
}