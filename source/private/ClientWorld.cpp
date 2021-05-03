#include "ClientWorld.h"
#include "RenderObject.h"
#include "UpdateObject.h"
#include <sstream>
#include "Player.h"
#include "GameManager.h"
#include "AsyncChunkManager.h"

ClientWorld::ClientWorld(ClientWorld& connectedWorld) : World(connectedWorld.metaData, connectedWorld.netClient)
{
	networkPlayers = connectedWorld.networkPlayers;
}

bool ClientWorld::TryConnect(const char* hostname, int port)
{
	InitialiseNetClient();
	
	ENetAddress address;
	ENetEvent event;
	enet_address_set_host(&address, hostname);
	address.port = port;
	ENetPeer* netHost = enet_host_connect(netClient, &address, 2, 0);
	networkPlayers.push_back(MakeShared<NetworkPlayer>(netHost, 0));
	if (netHost == nullptr)
	{
		DPrint("No available peers for initiating an ENet connection.");
		return false;
	}
	
	bool connected = false;
	while (enet_host_service(netClient, &event, 5000) > 0)
	{
		if (!connected && event.type == ENET_EVENT_TYPE_CONNECT)
		{
			DPrintf("Connection to %i:%i succeeded.\n", netHost->address.host, netHost->address.port);
			enet_host_flush(netClient);
			connected = true;
		}
		else if(event.type == ENET_EVENT_TYPE_RECEIVE)
		{
			metaData = *(WorldMetaData*)event.packet->data;
			DPrintf("Seed: %i, Region Length: %i\n", metaData.seed, metaData.regionLength);
			break;
		}
	}
	
	if(!connected)
	{
		enet_peer_reset(netHost);
		DPrintf("Connection to %i:%i failed.\n", address.host, address.port);
		netHost = nullptr;
	}

	return netHost;
}


void ClientWorld::InitialiseNetClient()
{
	if (enet_initialize() != 0)
	{
		DPrint("An error occurred while initializing ENet.");
		return;
	}
	
	netClient = enet_host_create(NULL /* create a client host */,
		1 /* only allow 1 outgoing connection */,
		2 /* allow up 2 channels to be used, 0 and 1 */,
		0 /* assume any amount of incoming bandwidth */,
		0 /* assume any amount of outgoing bandwidth */);
	if (netClient == nullptr)
	{
		DPrint("An error occurred while trying to create an ENet client host.");
		enet_deinitialize();
		return;
	}
}

void ClientWorld::Exit()
{
	DisconnectFromHost();
	chunkManager->Exit();

	auto& gameManager = GameManager::Instance();
	gameManager.ClearWorld();
	gameManager.CreateWorldLoader();
}

void ClientWorld::ExitNetClient()
{
	if (netClient)
	{
		DisconnectFromHost();
	}
}

void ClientWorld::DisconnectFromHost()
{
	if (auto& hostPlayer = GetHostPlayer())
	{
		if (ENetPeer* netHost = hostPlayer->GetNetPeer())
		{
			ENetEvent event;
			enet_peer_disconnect(netHost, 0);
			while (enet_host_service(netClient, &event, 3000) > 0)
			{
				switch (event.type)
				{
				case ENET_EVENT_TYPE_RECEIVE:
					enet_packet_destroy(event.packet);
					break;
				case ENET_EVENT_TYPE_DISCONNECT:
					enet_host_flush(netClient);
					DPrint("Disconnection succeeded.");
					hostPlayer->SetNetPeer(nullptr);
					return;
				}
			}
			enet_peer_reset(netHost);
			hostPlayer->SetNetPeer(nullptr);
		}
	}
}


void ClientWorld::Update(float deltaTime)
{
	World::Update(deltaTime);
}

void ClientWorld::UpdateGUI()
{
	World::UpdateGUI();
}

void ClientWorld::HandlePacket(ENetEvent& event)
{
	World::HandlePacket(event);
}

SharedPtr<Chunk> ClientWorld::RequestChunk(const glm::ivec3& chunkPosition)
{
	auto chunk = MakeShared<Chunk>(chunkPosition);
	ClientRequestChunk(chunkPosition);
	return chunk;
}

void ClientWorld::ClientRequestChunk(const glm::ivec3& chunkPosition)
{
	std::ostringstream os;
	os << "rc " << std::to_string(chunkPosition.x) << " " << std::to_string(chunkPosition.y) << " " << std::to_string(chunkPosition.z);
	SendPacket(os.str(), GetHostPlayer()->GetNetPeer(), 1);
}

void ClientWorld::BroadcastPacket(const enet_uint8* data, const int dataSize, const int channel)
{
	if (auto& hostPlayer = GetHostPlayer())
	{
		SendPacket(data, dataSize, hostPlayer->GetNetPeer(), channel);
	}
}

void ClientWorld::UpdateNetworkPlayerTransform(const ENetPeer* sender, const uint32_t uniqueId, const Transform<glm::vec3>& transform)
{
	if(!uniqueId)
	{
		GetHostPlayer()->SetTransform(transform);
	}
	else
	{
		for (auto& netPlayer : networkPlayers)
		{
			if (netPlayer->GetUniqueID() == uniqueId)
			{
				netPlayer->SetTransform(transform);
				break;
			}
		}
	}
}

void ClientWorld::OnPlayerDisconnect(const SharedPtr<NetworkPlayer> disconnectee)
{
	if(GetHostPlayer() == disconnectee)
	{
		doExit = true;
	}

	World::OnPlayerDisconnect(disconnectee);
}