#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform samplerCube skybox;
uniform vec3 cameraPos;

void main()
{    
    vec3 I = normalize(FragPos - cameraPos);
    vec3 R = normalize(reflect(Normal, I));
    FragColor = vec4(texture(skybox, R).rgb, 1.0f);
}