#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 normal;
    vec4 transformedPos;
} gs_in[];

const float MAGNITUDE = 0.2;

void GenerateLine(int index)
{
    gl_Position = gs_in[index].transformedPos;
    EmitVertex();
    gl_Position = gs_in[index].transformedPos + vec4(gs_in[index].normal, 0.0) * MAGNITUDE;
    EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateLine(0); // first vertex normal
    GenerateLine(1); // second vertex normal
    GenerateLine(2); // third vertex normal
}