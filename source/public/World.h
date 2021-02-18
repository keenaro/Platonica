#pragma once
#include "RenderObject.h"
#include "UpdateObject.h"
#include "Defs.h"
#include "AsyncChunkManager.h"
#include "PerlinNoise.h"

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

	SharedPtr<Player> GetPlayer() const { return player; };
	SharedPtr<ChunkRegion> GetOrCreateRegion(glm::ivec3& pos);

	AsyncChunkManager chunkManager;
	PerlinNoise perlin;

	bool TryIncrementChunkGenBufferCount();

private:
	void LoadTextureAtlas();
	void TryRequestChunks();

private:
	int regionLength;
	SharedMap3<ChunkRegion> regions;
	SharedPtr<Player> player;
	std::vector<glm::ivec3> chunkPositionsInRange;


//Optimisations
private:
	const int renderDistance = 10;
	const int maxChunkBufferGensPerTick = 100;
	int currentChunkBufferCount = 0;

public:
	World(World const&) = delete;
	void operator=(World const&) = delete;
};

