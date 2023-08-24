#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

const float offset = 1.0f / 300.0f;

void main()
{
    vec2 offsets[9] = vec2[](
	    vec2(-offset,  offset),
        vec2( 0.0f,    offset),
        vec2( offset,  offset),
        vec2(-offset,  0.0f),
        vec2( 0.0f,    0.0f),
        vec2( offset,  0.0f),   
        vec2(-offset, -offset), 
        vec2( 0.0f,   -offset), 
        vec2( offset, -offset) 
    );

    // Convolution Matrix
    // Change the matrix accordingly for use
    float kernel[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );

    // Sample with offsets
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++) 
        sampleTex[i] = vec3(texture(texture1, TexCoords.st + offsets[i]));
    
    // Combine effects
    vec3 color = vec3(0.0);
    for(int i = 0; i < 9; i++)
        color += sampleTex[i] * kernel[i];

    FragColor = vec4(color, 1.0);
}

// Inversion
//void main()
//{    
//    vec3 originalColor = texture(texture1, TexCoords).rgb;
//    FragColor = vec4((vec3(1.0f) - originalColor), 1.0f);
//}

// Grayscale
//void main()
//{
//    FragColor = texture(screenTexture, TexCoords);
//    float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
//    FragColor = vec4(average, average, average, 1.0);
//}