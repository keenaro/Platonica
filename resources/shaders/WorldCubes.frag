#version 330 core
out vec4 FragColor;

in vec3 VertexNormal;
in vec3 VertexPosition;

void main()
{
    FragColor = vec4(0.5, 0.5, 0.5, 1.0);
}