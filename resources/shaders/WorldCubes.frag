#version 330 core
out vec4 FragColor;

in vec3 VertexNormal;
in vec3 VertexPosition;
in vec2 TexCoord;
uniform sampler2D TextureAtlas;
uniform float TimeOfDay;

void main()
{
    vec3 shading = vec3(VertexNormal.x * 0.5f + VertexNormal.y * 0.75f + VertexNormal.z);
    
    FragColor = texture(TextureAtlas, TexCoord) * vec4(abs(shading) * TimeOfDay, 1.0);
}