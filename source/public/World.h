#pragma once
#include "RenderObject.h"
#include "UpdateObject.h"
#include "Defs.h"
#include "AsyncChunkManager.h"
#include "PerlinNoise.h"
#include "Chunk.h"

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
	World(int inRegionLength = 4);

public:
	int GetRegionLength() const { return regionLength; };
	int GetRenderDistance() const { return renderDistance * CHUNK_LENGTH; };
	int GetOffloadDistance() const { return GetRenderDistance() + CHUNK_LENGTH; }

	void Update(float deltaTime) override;
	void Draw() override;
	void SetShaderUniformValues() override;

	SharedPtr<Player> GetPlayer() const { return player; };
	SharedPtr<ChunkRegion> GetOrCreateRegion(const glm::ivec3& pos);

	AsyncChunkManager chunkManager;
	SharedPtr<PerlinNoise> perlin;

private:
	void DrawRegions() const;
	void UpdateRegions(float deltaTime) const;

	void TryRequestChunks();
	SharedPtr<Chunk> GetChunk(const glm::ivec3& chunkPosition) const;

	int TranslateIntoWrappedWorld(int value);
	glm::ivec3 TranslateIntoWrappedWorld(const glm::ivec3& vec3ToTranslate);

private:
	int regionLength;
	SharedIVec3Map<ChunkRegion> regions;
	SharedPtr<Player> player;

private:
	const int renderDistance = 8;

public:
	World(World const&) = delete;
	void operator=(World const&) = delete;
};

