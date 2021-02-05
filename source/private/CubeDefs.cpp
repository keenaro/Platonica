#include "CubeDefs.h"
#include "Defs.h"

CubeDefs::CubeDefs()
{
	RegisterCubeDef(CubeID::Stone, std::string("Stone"), glm::ivec2(1, 0));
	RegisterCubeDef(CubeID::Dirt, std::string("Dirt"), glm::ivec2(2, 0));
}

const CubeDef& CubeDefs::GetCubeDef(CubeID cubeID) const
{
	return cubeEntries.at(cubeID);
}

void CubeDefs::RegisterCubeDef(CubeID cubeID, std::string& cubeName, glm::ivec2& cubeTextureCoordinates)
{
	std::map<CubeID, CubeDef>::iterator search = cubeEntries.find(cubeID);
	if (search == cubeEntries.end())
	{
		cubeEntries[cubeID] = CubeDef(cubeName, cubeTextureCoordinates);
		DPRINTF("CubeDefs: Registered %s Cube id:%i successfully.\n", cubeName.c_str(), cubeID);
	}
	else
	{
		DPRINTF("CubeDefs: %s Cube id:%i has already been registered.\n", cubeName.c_str(), cubeID);
	}
}