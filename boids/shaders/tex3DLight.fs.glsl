#version 330 core

in vec3 vPosition_vs;
in vec3 vNormal_vs;
in vec2 vTexCoords;

uniform vec3 uKd;
uniform vec3 uKs;
uniform float uShininess;

uniform vec3 uPointLightPos_vs;
uniform vec3 uPointLightColor;
uniform float uPointLightIntensity;

uniform sampler2D uTexture;

out vec4 fFragColor;

vec3 blinnPhongPoint(vec3 position, vec3 normal, vec3 kd, vec3 ks, float shininess, vec3 uLightIntensity, vec3 uLightPos_vs) 
{
    vec3 viewDir = normalize(- position);
    vec3 lightDir = normalize(uLightPos_vs - position);
    float distanceToLight = length(uLightPos_vs - position);
    vec3 attenuatedLightIntensity = uLightIntensity / (distanceToLight * distanceToLight);
    vec3 halfVector = normalize(lightDir + viewDir);

    float diffuse = max(0.0, dot(normal, lightDir));
    float specular = pow(max(0.0, dot(normal, halfVector)), shininess);

    return kd * attenuatedLightIntensity * diffuse + ks * attenuatedLightIntensity * specular;
}

void main() 
{
    vec4 texture = texture(uTexture, vTexCoords);
    vec3 diffuseColor = texture.xyz;

    vec3 normal = normalize(vNormal_vs);
    vec3 color = blinnPhongPoint(vPosition_vs, normal, uKd, uKs, uShininess, uPointLightIntensity * uPointLightColor, uPointLightPos_vs);

    fFragColor = vec4(color * diffuseColor, texture.w);
}