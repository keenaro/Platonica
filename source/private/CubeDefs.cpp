#include "CubeDefs.h"
#include "Defs.h"

CubeDefs::CubeDefs()
{
	RegisterCubeDef(CubeID::Stone, std::string("Stone"), glm::ivec2(3, 0));
	RegisterCubeDef(CubeID::Dirt, std::string("Dirt"), glm::ivec2(0, 0));
	RegisterCubeDef(CubeID::Grass, std::string("Grass"), glm::ivec2(2, 0), glm::ivec2(0,0), glm::ivec2(1,0), glm::ivec2(1, 0), glm::ivec2(1, 0), glm::ivec2(1, 0));
}

const CubeDef& CubeDefs::GetCubeDef(CubeID cubeID) const
{
	return cubeEntries.at(cubeID);
}

void CubeDefs::RegisterCubeDef(CubeID cubeID, std::string& cubeName, glm::ivec2& topFaceTexCoord, glm::ivec2& bottomFaceTexCoord, glm::ivec2& leftFaceTexCoord, glm::ivec2& rightFaceTexCoord, glm::ivec2& frontFaceTexCoord, glm::ivec2& backFaceTexCoord)
{
	std::map<CubeID, CubeDef>::iterator search = cubeEntries.find(cubeID);
	if (search == cubeEntries.end())
	{
		cubeEntries[cubeID] = CubeDef(cubeName, topFaceTexCoord, bottomFaceTexCoord, leftFaceTexCoord, rightFaceTexCoord, frontFaceTexCoord, backFaceTexCoord);
		DPrintf("CubeDefs: Registered %s Cube id:%i successfully.\n", cubeName.c_str(), cubeID);
	}
	else
	{
		DPrintf("CubeDefs: %s Cube id:%i has already been registered.\n", cubeName.c_str(), cubeID);
	}
}

void CubeDefs::RegisterCubeDef(CubeID cubeID, std::string& cubeName, glm::ivec2& allFaceTexCoord)
{
	RegisterCubeDef(cubeID, cubeName, allFaceTexCoord, allFaceTexCoord, allFaceTexCoord, allFaceTexCoord, allFaceTexCoord, allFaceTexCoord);
}