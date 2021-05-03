#version 330 core
struct VertexStructure
{
  vec3 position;
  vec3 normal;
  vec2 texCoord;
};

layout (location = 0) in VertexStructure vertex;
uniform ivec3 RegionPosition;
uniform vec3 Position;

uniform vec3 CameraPosition;
uniform mat4 ProjectionXform;
uniform mat4 ViewWorldXform;
uniform float SphericalWorldFalloff;

void main()
{
	vec3 worldSpacePosition = vertex.position + Position + RegionPosition;
	float fallOffAmount = distance(vec2(CameraPosition.x, CameraPosition.z), vec2(worldSpacePosition.x, worldSpacePosition.z));
	worldSpacePosition -= vec3(0.0, pow(fallOffAmount,2)*SphericalWorldFalloff, 0.0);

	vec4 ViewPosition = ViewWorldXform * vec4(worldSpacePosition, 1.0);
	gl_Position = ProjectionXform * ViewPosition;
}