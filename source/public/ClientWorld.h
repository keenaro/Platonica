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
	void ExitNetClient() override;
	void DisconnectFromHost();
	bool IsHostWorld() const override { return false; };
	virtual void HandlePacket(ENetEvent& event);
	ENetPeer* GetNetHost() { return netPeers.empty() ? nullptr : *netPeers.begin(); }
	SharedPtr<Chunk> RequestChunk(const glm::ivec3& chunkPosition) override;
	void ClientRequestChunk(const glm::ivec3& chunkPosition);
	
};

