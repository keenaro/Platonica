#version 330 core
layout (location = 0) in int VertexData;

uniform vec3 CameraPosition;
uniform mat4 ProjectionXform;
uniform mat4 ViewWorldXform;
uniform ivec3 WorldOffsetPosition;
uniform ivec3 ChunkPosition;

out vec3 VertexNormal;
out vec3 VertexPosition;

ivec3 GetCubePositionFromCubeVertex(int data)
{
	int x = (data >> 24) & 15;
	int y = (data >> 20) & 15;
	int z = (data >> 16) & 15;

	return ivec3(x, y, z);
}

ivec3 GetNormalFromCubeVertex(int data)
{
	int sign = (data >> 31) * 2 + 1;
	int x = ((data >> 30) & 1);
	int y = ((data >> 29) & 1);
	int z = ((data >> 28) & 1);

	return ivec3(x, y, z) * sign;
}

ivec3 GetVertexPositionFromCubeVertex(int data)
{
	int x = (data >> 15) & 1;
	int y = (data >> 14) & 1;
	int z = (data >> 13) & 1;

	return ivec3(x, y, z);
}

ivec2 GetTextureCoordFromCubeVertex(int data)
{
	int x = (data >> 7) & 63;
	int y = (data >> 1) & 63;

	return ivec2(x, y);
}

// 0000 0000  0000  0000  000  000000  000000 0
// Norm CposX CposY CposZ Vpos TexCoX  TexCoY

void main()
{	
	ivec3 vertPos = GetVertexPositionFromCubeVertex(VertexData);
	ivec3 position = GetCubePositionFromCubeVertex(VertexData);
	ivec2 texCoord = GetTextureCoordFromCubeVertex(VertexData);

	VertexNormal = GetNormalFromCubeVertex(VertexData);

	vec3 worldSpacePosition = position + vertPos + ChunkPosition + WorldOffsetPosition;

	vec4 ViewPosition = ViewWorldXform * vec4(worldSpacePosition, 1.0);

    gl_Position = ProjectionXform * ViewPosition;
	VertexPosition = worldSpacePosition;
}