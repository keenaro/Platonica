#include "CubeDef.h"
#include "Cube.h"

CubeDef::CubeDef(std::string& cubeName, glm::ivec2& allTexCoord)
{
	name = cubeName;
	topTexCoord = bottomTexCoord = leftTexCoord = rightTexCoord = frontTexCoord = backTexCoord = ConvertToInt32TextureCoordinate(allTexCoord);
}

CubeDef::CubeDef(std::string& cubeName, glm::ivec2& topFaceTexCoord, glm::ivec2& bottomFaceTexCoord, glm::ivec2& leftFaceTexCoord, glm::ivec2& rightFaceTexCoord, glm::ivec2& frontFaceTexCoord, glm::ivec2& backFaceTexCoord)
{
	name = cubeName;
	topTexCoord = ConvertToInt32TextureCoordinate(topFaceTexCoord);
	bottomTexCoord = ConvertToInt32TextureCoordinate(bottomFaceTexCoord);
	leftTexCoord = ConvertToInt32TextureCoordinate(leftFaceTexCoord);
	rightTexCoord = ConvertToInt32TextureCoordinate(rightFaceTexCoord);
	frontTexCoord = ConvertToInt32TextureCoordinate(frontFaceTexCoord);
	backTexCoord = ConvertToInt32TextureCoordinate(backFaceTexCoord);
}

int32_t CubeDef::GetTextureCoordinatesFromNormal(const glm::ivec3 normal) const 
{
	if(normal.x)
	{
		return normal.x < 0 ? leftTexCoord : rightTexCoord;
	}
	else if(normal.y)
	{
		return normal.y < 0 ? bottomTexCoord : topTexCoord;
	}
	else
	{
		return normal.z < 0 ? backTexCoord : frontTexCoord;
	}
}

int32_t CubeDef::ConvertToInt32TextureCoordinate(glm::ivec2& texCoord) const
{
	return ((texCoord.x & 63) << 7) + ((texCoord.y & 63) << 1);
}