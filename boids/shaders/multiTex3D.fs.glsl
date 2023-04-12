#version 330

in vec2 vTexCoords;

uniform sampler2D uTexture1;
uniform sampler2D uTexture2;

out vec4 fFragColor;

void main() {
    fFragColor = vec4(vec3(texture(uTexture1, vTexCoords)) + vec3(texture(uTexture2, vTexCoords)), 1);
}