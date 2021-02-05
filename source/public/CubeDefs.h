#pragma once
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <map>
#include <string>
#include "CubeDef.h"

enum CubeID
{
	Air = 0,
	Stone = 1,
	Dirt = 2,
	Grass = 3
};

class CubeDefs
{
public:
	static CubeDefs& Instance()
	{
		static CubeDefs instance;
		return instance;
	}

public:
	const CubeDef& GetCubeDef(CubeID cubeID) const;

private:
	std::map<CubeID, CubeDef> cubeEntries;

	CubeDefs();
	void RegisterCubeDef(CubeID cubeID, std::string& cubeName, glm::ivec2& cubeTextureCoordinates);

public:
	CubeDefs(CubeDefs const&) = delete;
	void operator=(CubeDefs const&) = delete;
};
