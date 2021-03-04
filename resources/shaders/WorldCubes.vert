#version 330 core
layout (location = 0) in int VertexData;

uniform vec3 CameraPosition;
uniform mat4 ProjectionXform;
uniform mat4 ViewWorldXform;
uniform ivec3 RegionPosition;
uniform ivec3 ChunkPosition;
uniform float SphericalWorldFalloff;

out vec3 VertexNormal;
out vec3 VertexPosition;
out vec2 TexCoord;

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
	VertexNormal = GetNormalFromCubeVertex(VertexData);

	ivec3 vertPos = GetVertexPositionFromCubeVertex(VertexData);
	ivec3 position = GetCubePositionFromCubeVertex(VertexData);

	const int atlasLength = 64;

	vec3 inverseVertPos = abs(vec3(1) - vertPos);

	vec3 maxNormal = max(vec3(0), -VertexNormal);
	vec3 maxFlippedNormal = abs(min(vec3(0.0), -VertexNormal));

	vec2 xTexCoord = vec2(vertPos.z, inverseVertPos.y) / atlasLength * maxNormal.x;
	vec2 yTexCoord = vec2(inverseVertPos.x, vertPos.z) / atlasLength * maxNormal.y;
	vec2 zTexCoord = vec2(inverseVertPos.x, inverseVertPos.y) / atlasLength * maxNormal.z;
	vec2 xTexCoordFlipped = vec2(inverseVertPos.z, inverseVertPos.y) / atlasLength * maxFlippedNormal.x;
	vec2 yTexCoordFlipped = vec2(inverseVertPos.x, inverseVertPos.z) / atlasLength * maxFlippedNormal.y;
	vec2 zTexCoordFlipped = vec2(vertPos.x, inverseVertPos.y) / atlasLength * maxFlippedNormal.z;

	vec2 startTexCoord = vec2(GetTextureCoordFromCubeVertex(VertexData)) / 64;
	TexCoord = startTexCoord + xTexCoord + yTexCoord + zTexCoord + xTexCoordFlipped + yTexCoordFlipped + zTexCoordFlipped;

	vec3 worldSpacePosition = position + vertPos + ChunkPosition + RegionPosition;
	float fallOffAmount = distance(vec2(CameraPosition.x, CameraPosition.z), vec2(worldSpacePosition.x, worldSpacePosition.z)) * SphericalWorldFalloff;
	worldSpacePosition -= vec3(0.0, fallOffAmount*fallOffAmount, 0.0);

	vec4 ViewPosition = ViewWorldXform * vec4(worldSpacePosition, 1.0);

    gl_Position = ProjectionXform * ViewPosition;
	VertexPosition = worldSpacePosition;
}