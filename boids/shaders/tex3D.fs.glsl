#version 330

in vec2 vTexCoords;

uniform sampler2D uTexture;

void main() {
    fFragColor = vec3(texture(uTexture, vTexCoords));
}