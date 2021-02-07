#version 330 core
layout (location = 0) in vec3 aPos;
uniform vec3 VertexPosition;
out vec3 colour;

void main()
{
   gl_Position = vec4(aPos + VertexPosition, 1.0);
   colour = aPos + VertexPosition + vec3(0.5);
}