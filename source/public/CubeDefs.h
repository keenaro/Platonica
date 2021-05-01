#pragma once
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <map>
#include <string>
#include "CubeDef.h"

enum CubeID : uint16_t
{
	Air = 0,
	Stone = 1,
	Dirt = 2,
	Grass = 3,
	Wood = 4,
	RuggedStone = 5,
	Leaves = 6,
	Ice = 7,
	Water = 8,
	Bricks_Red = 9,
	Sand = 10,
	Cactus = 11,
	Bricks_Black = 12,
	Planks_Birch = 13,
	Planks_Pine = 14,
	Planks_Oak = 15,
	Chest = 16,
	Furnace = 17,

	ENUM_MAX
};

class CubeDefs
{
public:
	CubeDefs();

public:
	const CubeDef& GetCubeDef(CubeID cubeID) const;
	bool ShouldAffectCull(CubeID id) const { return id != Air; };

	const std::map<CubeID, CubeDef>& GetCubeDefsMap() const { return cubeEntries; };

private:
	std::map<CubeID, CubeDef> cubeEntries;

	void RegisterCubeDef(CubeID cubeID, std::string& cubeName, glm::ivec2& allFaceTexCoord);
	void RegisterCubeDef(CubeID cubeID, std::string& cubeName, glm::ivec2& topFaceTexCoord, glm::ivec2& bottomFaceTexCoord, glm::ivec2& leftFaceTexCoord, glm::ivec2& rightFaceTexCoord, glm::ivec2& frontFaceTexCoord, glm::ivec2& backFaceTexCoord);
};
