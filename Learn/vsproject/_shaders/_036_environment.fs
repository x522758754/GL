#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;
in vec2 TexCoords;

uniform vec3 cameraPos;
uniform samplerCube skybox;
uniform sampler2D texture_diffuse1;

void main()
{             
    vec3 I = normalize(Position - cameraPos);
    vec3 direReflect = reflect(I, normalize(Normal));
    vec3 colorReflect = texture(skybox, direReflect).rgb;

    float ratio = 1.00 / 1.52;
    vec3 direRefract = refract(I, normalize(Normal), ratio);
    vec3 colorRefract = texture(skybox, direRefract).rgb;

    vec3 colorTexture = texture(texture_diffuse1, TexCoords).rgb;

    FragColor = vec4(colorReflect + colorRefract + colorTexture, 1.0);
}