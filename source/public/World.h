#pragma once
#include "RenderObject.h"
#include "UpdateObject.h"
#include "Defs.h"
#include "AsyncChunkManager.h"

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

private:
	void LoadTextureAtlas();

private:
	int regionLength;
	SharedMap3<ChunkRegion> regions;
	SharedPtr<Player> player;

public:
	World(World const&) = delete;
	void operator=(World const&) = delete;
};

