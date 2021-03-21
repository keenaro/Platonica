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
	World(int inRegionLength = 8);

public:
	int GetRegionLength() const { return regionLength; };
	int GetRenderDistance() const { return renderDistance * CHUNK_LENGTH; };
	int GetOffloadDistance() const { return GetRenderDistance() + CHUNK_LENGTH; }
	void PlaceBlockFromPositionInDirection(const glm::vec3& position, const glm::vec3& direction, float maxDistance, CubeID blockToPlace, bool shouldReplaceBlock = false);

	void Update(float deltaTime) override;
	void Draw() override;

	glm::ivec3 TranslateIntoWrappedWorld(const glm::ivec3& vec3ToTranslate) const;

	SharedPtr<Player> GetPlayer() const { return player; };
	SharedPtr<ChunkRegion> GetOrCreateRegion(const glm::ivec3& pos);
	SharedPtr<Chunk> FindChunk(const glm::ivec3& chunkPosition) const;

	bool DidPlayerMoveChunk() const { return playerMovedChunk; };

	SharedPtr<PerlinNoise> GetPerlinNoiseGenerator() const { return perlin; };
	
	String GetWorldDirectory() const { return "Worlds/" + worldName; };
	String GetWorldChunkDataDirectory() const { return "Worlds/" + worldName + "/Chunks/"; }
	SharedPtr<AsyncChunkManager> GetChunkManager() const { return chunkManager; };

private:
	void DrawRegions() const;
	void UpdateRegions(float deltaTime);
	void SetShaderUniformValues() override;

	void TryRequestChunks();
	SharedPtr<Chunk> GetOrCreateChunkFromWorldPosition(const glm::vec3& position, bool createIfNull = false);
	void GetBlockWorldPosition(const glm::vec3& position, glm::ivec3& outBlockWorldPosition);

	int TranslateIntoWrappedWorld(int value) const;
	void UpdatePlayerHasMovedChunk();

	void UpdateGUI();

	void InitialiseSaveDirectories();
private:
	int regionLength;
	SharedIVec3Map<ChunkRegion> regions;
	SharedPtr<Player> player;
	SharedPtr<AsyncChunkManager> chunkManager;
	SharedPtr<PerlinNoise> perlin;
	glm::ivec3 cachedPlayerChunkPosition;
	bool playerMovedChunk = false;

private:
	int renderDistance = 8;
	float sphericalFalloff = 0.005f;
	String worldName = "World";

public:
	World(World const&) = delete;
	void operator=(World const&) = delete;
};

