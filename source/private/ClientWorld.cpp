#include "ClientWorld.h"
#include "RenderObject.h"
#include "UpdateObject.h"
#include <sstream>

ClientWorld::ClientWorld(ClientWorld& connectedWorld) : World(connectedWorld.seed, connectedWorld.regionLength, connectedWorld.netClient)
{
	netPeers = connectedWorld.netPeers;
}

bool ClientWorld::TryConnect(const char* hostname, int port)
{
	InitialiseNetClient();
	
	ENetAddress address;
	ENetEvent event;
	enet_address_set_host(&address, hostname);
	address.port = port;
	ENetPeer* netHost = enet_host_connect(netClient, &address, 2, 0);
	netPeers.insert(netHost);
	if (netHost == nullptr)
	{
		DPrint("No available peers for initiating an ENet connection.");
		return false;
	}
	
	bool connected = false;
	/* Wait up to 5 seconds for the connection attempt to succeed. */
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
			uint32_t data = *(uint32_t*)event.packet->data;
			seed = data >> 16;
			regionLength = data & 255;
			DPrintf("Seed: %i, Region Length: %i\n", seed, regionLength);
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

void ClientWorld::ExitNetClient()
{
	if (netClient)
	{
		DisconnectFromHost();
	}
}

void ClientWorld::DisconnectFromHost()
{
	ENetPeer* netHost = GetNetHost();
	if (netHost)
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
				netHost = nullptr;
				return;
			}
		}
		enet_peer_reset(netHost);
		netHost = nullptr;
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
	SendPacket(os.str(), GetNetHost(), 1);
}