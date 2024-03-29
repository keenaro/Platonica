#pragma once
#include "Defs.h"
#include "RenderObject.h"
#include "UpdateObject.h"
#include "PerlinNoise.h"
#include "Chunk.h"
#include <unordered_set>
#include "Transform.h"

class ChunkRegion;
class Player;
class NetworkPlayer;
class AsyncChunkManager;

struct WorldMetaData
{
public:
	uint16_t seed = 0;
	uint16_t regionLength = 0;
	float timeOfDay = 0;
};

class World : public RenderObject, public UpdateObject
{
public:
	World(const String& inWorldName, uint16_t inSeed, uint16_t inRegionLength);
	World(const WorldMetaData& inMetaData, ENetHost* inClient);
	World() : RenderObject(false), UpdateObject(false) {};
public:
	int GetRegionLength() const { return metaData.regionLength; };
	int GetRenderDistance() const { return renderDistance * CHUNK_LENGTH; };
	int GetOffloadDistance() const { return GetRenderDistance() + CHUNK_LENGTH; }
	void PlaceBlockFromPositionInDirection(const glm::vec3& position, const glm::vec3& direction, float maxDistance, CubeID blockToPlace, bool shouldReplaceBlock = false);
	void PlaceBlockAtPosition(const glm::ivec3& position, const CubeID cubeId, bool shouldReplicate = true);

	glm::ivec3 TranslateIntoWrappedWorld(const glm::ivec3& vec3ToTranslate) const;
	glm::vec3 TranslateIntoWrappedWorld(const glm::vec3& vec3ToTranslate) const;

	SharedPtr<Player> GetPlayer() const { return player; };
	SharedPtr<ChunkRegion> GetOrCreateRegion(const glm::ivec3& pos);
	SharedPtr<Chunk> FindChunk(const glm::ivec3& chunkPosition) const;

	bool DidPlayerMoveChunk() const { return playerMovedChunk; };
	void SendLocalPlayerPosition();
	void BroadcastPlayerPosition(SharedPtr<NetworkPlayer> netPlayer);

	SharedPtr<PerlinNoise> GetPerlinNoiseGenerator() const { return perlin; };
	
	String GetWorldDirectory() const { return "Worlds/" + worldName; };
	String GetWorldChunkDataDirectory() const { return GetWorldDirectory() + "/Chunks/"; }
	String GetWorldMetadataDirectory() const { return GetWorldDirectory() + "/meta.dat"; }
	String GetChunkDataFile(const glm::ivec3& chunkPosition) const { return GetWorldChunkDataDirectory() + std::to_string(chunkPosition.x) + "_" + std::to_string(chunkPosition.y) + "_" + std::to_string(chunkPosition.z) + ".cd"; }
	SharedPtr<AsyncChunkManager> GetChunkManager() const { return chunkManager; };

	virtual bool IsHostWorld() const { return true; };

	virtual void Exit();
	virtual void Update(float deltaTime) override;
	void Draw() override;

	const SharedIVec3Map<ChunkRegion>& GetRegions() const { return regions; };

protected:
	void DrawEntities() const;
	void DrawRegions() const;
	void UpdateRegions(float deltaTime);
	void SetShaderUniformValues() override;

	void TryRequestChunks();
	SharedPtr<Chunk> GetOrCreateChunkFromWorldPosition(const glm::vec3& position, bool createIfNull = false);
	void GetBlockWorldPosition(const glm::vec3& position, glm::ivec3& outBlockWorldPosition);

	int TranslateIntoWrappedWorld(int value) const;

	void UpdatePlayerHasMovedChunk();
	void UpdateTimeOfDay(float deltaTime);

	virtual void UpdateGUI();
	virtual char* GetGUIWindowName() const { return "Host World"; }

	virtual bool TryLoadMetaData();
	virtual void SaveMetaData();
	virtual void SaveChunks();

	virtual void PlaceBlock(SharedPtr<Chunk> chunk, const glm::ivec3& blockPosition, const CubeID cubeId, bool shouldReplicate = true);
	virtual void PlaceBlockInUnloadedChunk(const glm::ivec3& chunkPosition, const glm::ivec3& blockPosition, const CubeID cubeId, bool shouldReplicate = true);
	virtual void ReplicatePlaceBlock(const glm::ivec3& chunkWorldPosition, const glm::ivec3& blockPosition, const CubeID cubeId);
	virtual void InitialiseNetClient();
	virtual void ExitNetClient();
	virtual void UpdateNetClient();
	virtual void HandlePacket(ENetEvent& event);
	void SendChunkData(const glm::ivec3& chunkPosition, ENetPeer* peer);
	void SendPacket(const String& data, ENetPeer* peer, const int channel = 0);
	void SendPacket(const enet_uint8* data, const int dataSize, ENetPeer* peer, const int channel);
	virtual void BroadcastPacket(const enet_uint8* data, const int dataSize, const int channel);
	virtual SharedPtr<Chunk> RequestChunk(const glm::ivec3& chunkPosition);
	void GenerateChunkFromPacket(const glm::ivec3 chunkPosition, const std::vector<ChunkBlockData>& data);

	virtual void UpdateNetworkPlayerTransform(const ENetPeer* sender, const uint32_t uniqueId, const Transform<glm::vec3>& transform);
	bool HandlePlayerPositionPacket(const std::vector<String>& words, const ENetEvent& event);
	bool HandlePlaceBlockPacket(const std::vector<String>& words, const ENetEvent& event);
	bool HandleRequestChunkPacket(const std::vector<String>& words, const ENetEvent& event);
	bool HandleGenerateChunkPacket(const std::vector<String>& words, const ENetEvent& event);
	bool HandlePlayerJoinPacket(const std::vector<String>& words, const ENetEvent& event);
	bool HandlePlayerLeavePacket(const std::vector<String>& words, const ENetEvent& event);
	virtual void OnPlayerDisconnect(const SharedPtr<NetworkPlayer> disconnectee);
	void BroadcastPlayerLeave(const enet_uint32& uniqueId);
	void BroadcastPlayerConnection(const enet_uint32& uniqueId);
	void InformPlayerConnections(SharedPtr<NetworkPlayer> informee);
protected:
	SharedIVec3Map<ChunkRegion> regions;
	SharedPtr<Player> player;
	SharedPtr<AsyncChunkManager> chunkManager;
	SharedPtr<PerlinNoise> perlin;
	ENetHost* netClient = nullptr;

	glm::ivec3 cachedPlayerChunkPosition;
	bool playerMovedChunk = false;
	std::vector<SharedPtr<NetworkPlayer>> networkPlayers;

	SharedPtr<Shader> entityShader;
	enet_uint32 lastPlayerUniqueId = 0;
	bool doExit = false;


protected:
	int renderDistance = 8;
	float sphericalFalloff = 0.005f;
	String worldName = "World";
	WorldMetaData metaData;
	glm::vec3 skyColour = glm::vec3(0.3f, 0.6f, 0.8f);
	const float dayDuration = 60.0f;
};
