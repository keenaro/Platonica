#pragma once
#include "World.h"

class ClientWorld : public World
{
public:
	ClientWorld() {};
	ClientWorld(ClientWorld& connectedWorld);
public:
	void Update(float deltaTime) override;

	bool TryConnect(const char* hostname, int port);

protected:
	void UpdateGUI() override;

	bool TryLoadMetaData() override { return false; };
	void SaveMetaData() override {};
	void SaveChunks() override {};
	char* GetGUIWindowName() const override { return "Client World"; };
	void InitialiseNetClient() override;
	void Exit() override;
	void ExitNetClient() override;
	void DisconnectFromHost();
	bool IsHostWorld() const override { return false; };
	virtual void HandlePacket(ENetEvent& event);
	SharedPtr<NetworkPlayer> GetHostPlayer() { return networkPlayers.empty() ? nullptr : networkPlayers[0]; }
	SharedPtr<Chunk> RequestChunk(const glm::ivec3& chunkPosition) override;
	void ClientRequestChunk(const glm::ivec3& chunkPosition);

	void BroadcastPacket(const enet_uint8* data, const int dataSize, const int channel) override;
	void UpdateNetworkPlayerTransform(const ENetPeer* sender, const enet_uint32 uniqueId, const Transform<glm::vec3>& transform) override;
	void OnPlayerDisconnect(const SharedPtr<NetworkPlayer> disconnectee) override;

};

