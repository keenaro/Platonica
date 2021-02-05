#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <string>

class CubeDef
{
public:
	CubeDef() {};
	CubeDef(std::string& cubeName, glm::ivec2& allTexCoord);
	CubeDef(std::string& cubeName, glm::ivec2& topFaceTexCoord, glm::ivec2& bottomFaceTexCoord, glm::ivec2& leftFaceTexCoord, glm::ivec2& rightFaceTexCoord, glm::ivec2& frontFaceTexCoord, glm::ivec2& backFaceTexCoord);

	int32_t GetTextureCoordinatesFromNormal(glm::ivec3 normal) const;
	const std::string& GetName() const { return name; };

private:
	int32_t ConvertToInt32TextureCoordinate(glm::ivec2& texCoord) const;

	std::string name;
	int32_t bottomTexCoord, topTexCoord, leftTexCoord, rightTexCoord, frontTexCoord, backTexCoord;
};
