#pragma once
#include "RenderObject.h"
#include "UpdateObject.h"
#include "Defs.h"

class ChunkRegion;
class Player;

class World : public RenderObject, public UpdateObject
{
public:
	static World& Instance()
	{
		static World instance;
		return instance;
	}

public:
	World(int inRegionLength = 32);

public:
	int GetRegionLength() const { return regionLength; };

	void Update(float deltaTime) override;
	void Draw() override;
	void SetShaderUniformValues() override;

	void GenerateChunk(glm::ivec3 pos);

	SharedPtr<Player> GetPlayer() const { return player; };

private:
	SharedPtr<ChunkRegion> GetOrCreateRegion(glm::ivec3 pos);

private:
	int regionLength;
	SharedMap3<ChunkRegion> regions;
	SharedPtr<Player> player;

public:
	World(World const&) = delete;
	void operator=(World const&) = delete;
};

