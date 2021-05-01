#include "CubeDefs.h"
#include "Defs.h"

CubeDefs::CubeDefs()
{
	RegisterCubeDef(CubeID::Stone, std::string("Stone"), glm::ivec2(3, 0));
	RegisterCubeDef(CubeID::Dirt, std::string("Dirt"), glm::ivec2(0, 0));
	RegisterCubeDef(CubeID::Grass, std::string("Grass"), glm::ivec2(2, 0), glm::ivec2(0,0), glm::ivec2(1,0), glm::ivec2(1, 0), glm::ivec2(1, 0), glm::ivec2(1, 0));
	RegisterCubeDef(CubeID::Wood, std::string("Wood"), glm::ivec2(4, 0));
	RegisterCubeDef(CubeID::RuggedStone, std::string("Rugged Stone"), glm::ivec2(5, 0));
	RegisterCubeDef(CubeID::Leaves, std::string("Leaves"), glm::ivec2(6, 0));
	RegisterCubeDef(CubeID::Ice, std::string("Ice"), glm::ivec2(7, 0));
	RegisterCubeDef(CubeID::Water, std::string("Water"), glm::ivec2(8, 0));
	RegisterCubeDef(CubeID::Bricks_Red, std::string("Red Bricks"), glm::ivec2(9, 0));
	RegisterCubeDef(CubeID::Sand, std::string("Sand"), glm::ivec2(10, 0));
	RegisterCubeDef(CubeID::Cactus, std::string("Cactus"), glm::ivec2(11, 0), glm::ivec2(11, 0), glm::ivec2(12, 0), glm::ivec2(12, 0), glm::ivec2(12, 0), glm::ivec2(12, 0));
	RegisterCubeDef(CubeID::Bricks_Black, std::string("Black Bricks"), glm::ivec2(13, 0));
	RegisterCubeDef(CubeID::Planks_Birch, std::string("Birch Planks"), glm::ivec2(14, 0));
	RegisterCubeDef(CubeID::Planks_Pine, std::string("Pine Planks"), glm::ivec2(15, 0));
	RegisterCubeDef(CubeID::Planks_Oak, std::string("Oak Planks"), glm::ivec2(16, 0));
	RegisterCubeDef(CubeID::Chest, std::string("Chest"), glm::ivec2(17, 0), glm::ivec2(17, 0), glm::ivec2(18, 0), glm::ivec2(18, 0), glm::ivec2(19, 0), glm::ivec2(20, 0));
	RegisterCubeDef(CubeID::Furnace, std::string("Furnace"), glm::ivec2(21, 0), glm::ivec2(21, 0), glm::ivec2(22, 0), glm::ivec2(22, 0), glm::ivec2(23, 0), glm::ivec2(22, 0));
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