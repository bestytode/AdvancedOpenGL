#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main()
{
    FragColor = 0.8 * texture(texture_diffuse1, TexCoords) + 0.2 * texture(texture_specular1, TexCoords);
} 