#include "World.h"
#include "ChunkRegion.h"
#include "Player.h"
#include <algorithm>
#include "PerlinNoise.h"
#include "MathFunctions.h"
#include <iostream>
#include <fstream>
#include <direct.h>
#include "AsyncChunkManager.h"
#include <set>
#include <limits>
#include "ClientWorld.h"
#include <sstream>
#include "Window.h"
#include <algorithm>
#include "GameManager.h"

World::World(const String& inWorldName, uint16_t inSeed, uint16_t inRegionLength) : RenderObject(true), UpdateObject(true)
{
	worldName = inWorldName;

	if(!TryLoadMetaData())
	{
		metaData.seed = inSeed;
		metaData.regionLength = inRegionLength;
		metaData.timeOfDay = dayDuration / 2;
		SaveMetaData();
	}

	shader = ShaderLibrary::GetShader(std::string("WorldCubes"));
	entityShader = ShaderLibrary::GetShader(std::string("RegionEntity"));
	player = MakeShared<Player>();
	PerlinNoise::Reseed(metaData.seed);
	LoadTexture("textures/atlas.png");
	chunkManager = MakeShared<AsyncChunkManager>(4);

	InitialiseNetClient();
}

World::World(const WorldMetaData& inMetaData, ENetHost* inClient) : RenderObject(true), UpdateObject(true)
{
	metaData = inMetaData;
	netClient = inClient;

	shader = ShaderLibrary::GetShader(std::string("WorldCubes"));
	entityShader = ShaderLibrary::GetShader(std::string("RegionEntity"));
	player = MakeShared<Player>();
	PerlinNoise::Reseed(metaData.seed);
	LoadTexture("textures/atlas.png");
	chunkManager = MakeShared<AsyncChunkManager>(4);
}

void World::InitialiseNetClient()
{
	if (enet_initialize() != 0)
	{
		DPrint("An error occurred while initializing ENet.");
		return;
	}
	
	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = 25565;
	netClient = enet_host_create(&address /* the address to bind the server host to */,
		32      /* allow up to 32 clients and/or outgoing connections */,
		2      /* allow up to 2 channels to be used, 0 and 1 */,
		0      /* assume any amount of incoming bandwidth */,
		0      /* assume any amount of outgoing bandwidth */);
	
	if (netClient == nullptr)
	{
		DPrint("An error occurred while trying to create an ENet server host.");
		enet_deinitialize();
		return;
	}
	
	DPrintf("Successfully created a server on port %i.\n", address.port);
}

void World::ExitNetClient()
{
	if (netClient)
	{
		enet_host_destroy(netClient);
		enet_deinitialize();
		netClient = nullptr;
		DPrint("Destroyed client.");
	}
}

void World::Draw() 
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	RenderObject::Draw();
	DrawRegions();
	glBindTexture(GL_TEXTURE_2D, 0);
	DrawEntities();
}

void World::DrawEntities() const
{
	const glm::mat4 worldXform = glm::mat4(1);
	const glm::mat4 viewWorldXform = player->GetViewXForm() * worldXform;
	entityShader->Use();
	entityShader->SetVector3("CameraPosition", player->GetPosition());
	entityShader->SetMatrix4("ViewWorldXform", viewWorldXform);
	entityShader->SetMatrix4("ProjectionXform", player->GetProjectionXForm());
	entityShader->SetFloat("SphericalWorldFalloff", sphericalFalloff);

	for (auto& netPlayer : networkPlayers)
	{
		netPlayer->DrawPlayer();
	}
}

void World::DrawRegions() const
{
	for (auto& it : regions)
	{
		if (SharedPtr<ChunkRegion> region = it.second)
		{
			region->Draw();
		}
	}
}

void World::Update(float deltaTime)
{
	UpdateTimeOfDay(deltaTime);
	UpdateNetClient();
	player->Update(deltaTime);
	UpdatePlayerHasMovedChunk();
	UpdateRegions(deltaTime);
	TryRequestChunks();
	UpdateGUI();

	if (doExit) Exit();
}

void World::UpdateTimeOfDay(float deltaTime)
{
	metaData.timeOfDay = fmodf(metaData.timeOfDay + deltaTime, dayDuration);
	const float pi = glm::pi<float>();
	const float lerpProjection = (-sin((metaData.timeOfDay / dayDuration) * pi * 2 + pi/4) + 1.0f) / 2.0f;
	Window::Instance().SetClearColour(glm::lerp(glm::vec3(0), skyColour, lerpProjection));
}

void World::UpdatePlayerHasMovedChunk()
{
	const glm::ivec3& playerChunkPosition = RoundDownToMultiple(player->GetPosition(), CHUNK_LENGTH) / CHUNK_LENGTH;

	if (playerChunkPosition != cachedPlayerChunkPosition)
	{
		cachedPlayerChunkPosition = playerChunkPosition;
		playerMovedChunk = true;
	}
	else
	{
		playerMovedChunk = false;
	}
}

void World::UpdateRegions(float deltaTime)
{
	for (auto& it = regions.cbegin(); it != regions.cend();)
	{
		SharedPtr<ChunkRegion> region = it->second;
		if (!region || !region->GetChunkCount())
		{
			regions.erase(it++);
		}
		else
		{
			region->Update(deltaTime);
			++it;
		}
	}
}


void World::TryRequestChunks()
{
	const glm::ivec3& playerWorldPosition = player->GetPosition();
	const glm::ivec3& playerChunkPosition = RoundDownToMultiple(playerWorldPosition, CHUNK_LENGTH) / CHUNK_LENGTH;

	const int renderRadius = renderDistance / 2;
	for (int z = -renderRadius; z < renderRadius; z++)
	{
		for (int y = -renderRadius; y < renderRadius; y++)
		{
			for (int x = -renderRadius; x < renderRadius; x++)
			{
				const glm::ivec3& unwrappedChunkPosition = playerChunkPosition + glm::ivec3(x, y, z);
				const glm::ivec3& unwrappedChunkWorldPosition = unwrappedChunkPosition * CHUNK_LENGTH;
				const glm::ivec3& chunkWorldPosition = TranslateIntoWrappedWorld(unwrappedChunkWorldPosition);
				const glm::ivec3& chunkPosition = chunkWorldPosition / CHUNK_LENGTH;
				const glm::ivec3& regionPositionWorldSpace = RoundDownToMultiple(unwrappedChunkWorldPosition, metaData.regionLength * CHUNK_LENGTH) * glm::ivec3(1, 0, 1);
				const glm::ivec3& regionPosition = regionPositionWorldSpace / (CHUNK_LENGTH * metaData.regionLength);

				if (glm::distance(glm::vec3(playerWorldPosition), glm::vec3(unwrappedChunkWorldPosition)) < GetOffloadDistance())
				{
					const SharedPtr<ChunkRegion> region = GetOrCreateRegion(regionPosition);
					if (!region->GetChunk(chunkPosition))
					{
						SharedPtr<Chunk> chunk = FindChunk(chunkPosition);
						if (!chunk)
						{
							DPrintf("Requesting chunk at Region(%s) Chunk(%s)\n", glm::to_string(regionPosition).c_str(), glm::to_string(chunkPosition).c_str());
							chunk = RequestChunk(chunkPosition);
						}

						region->InsertChunk(chunk);
					}
				}
			}
		}
	}
}

SharedPtr<Chunk> World::RequestChunk(const glm::ivec3& chunkPosition)
{
	auto chunk = MakeShared<Chunk>(chunkPosition);
	chunkManager->RequestTask(MakeShared<GenerateJob>(chunk));
	return chunk;
}

SharedPtr<Chunk> World::FindChunk(const glm::ivec3& chunkPosition) const
{
	for (auto const& [position, region] : regions)
	{
		if (SharedPtr<Chunk> chunk = region->GetChunk(chunkPosition))
		{
			return chunk;
		}
	}

	return nullptr;
}

SharedPtr<ChunkRegion> World::GetOrCreateRegion(const glm::ivec3& pos)
{
	if (!regions[pos])
	{
		SharedPtr<ChunkRegion> region = MakeShared<ChunkRegion>(pos);
		regions[pos] = region;
	}

	return regions[pos];
}

void World::SetShaderUniformValues()
{
	const glm::mat4 worldXform = glm::mat4(1);
	const glm::mat4 viewWorldXform = player->GetViewXForm() * worldXform;

	shader->SetVector3("CameraPosition", player->GetPosition());
	shader->SetMatrix4("ViewWorldXform", viewWorldXform);
	shader->SetMatrix4("ProjectionXform", player->GetProjectionXForm());
	shader->SetFloat("SphericalWorldFalloff", sphericalFalloff);
	shader->SetInt("TextureAtlas", 0);

	const float pi = glm::pi<float>();
	const float lerpProjection = (-sin((metaData.timeOfDay / dayDuration) * pi * 2 + pi / 4) + 1.0f) / 2.0f * 0.8f + 0.2f;
	shader->SetFloat("TimeOfDay", lerpProjection );
}

int World::TranslateIntoWrappedWorld(int value) const
{
	const int regionWorldLength = metaData.regionLength * CHUNK_LENGTH;
	return (value + regionWorldLength * (abs(value / regionWorldLength) + 1)) % regionWorldLength;
}

glm::vec3 World::TranslateIntoWrappedWorld(const glm::vec3& vec3ToTranslate) const
{
	return glm::vec3(TranslateIntoWrappedWorld(vec3ToTranslate.x), (int)vec3ToTranslate.y, TranslateIntoWrappedWorld(vec3ToTranslate.z)) + vec3ToTranslate - glm::trunc(vec3ToTranslate);
}

glm::ivec3 World::TranslateIntoWrappedWorld(const glm::ivec3& vec3ToTranslate) const
{
	return glm::ivec3(TranslateIntoWrappedWorld(vec3ToTranslate.x), vec3ToTranslate.y, TranslateIntoWrappedWorld(vec3ToTranslate.z));
}

void World::UpdateGUI()
{
	ImGui::Begin(GetGUIWindowName());

	const glm::ivec3& playerWorldPosition = player->GetPosition();
	const glm::ivec3& playerChunkPosition = RoundDownToMultiple(playerWorldPosition, CHUNK_LENGTH) / CHUNK_LENGTH;
	const glm::ivec3& unwrappedChunkPosition = playerChunkPosition;
	const glm::ivec3& unwrappedChunkWorldPosition = unwrappedChunkPosition * CHUNK_LENGTH;
	const glm::ivec3& chunkWorldPosition = TranslateIntoWrappedWorld(unwrappedChunkWorldPosition);
	const glm::ivec3& chunkPosition = chunkWorldPosition / CHUNK_LENGTH;
	const glm::ivec3& regionPositionWorldSpace = RoundDownToMultiple(unwrappedChunkWorldPosition, metaData.regionLength * CHUNK_LENGTH) * glm::ivec3(1, 0, 1);
	const glm::ivec3& regionPosition = regionPositionWorldSpace / (CHUNK_LENGTH * metaData.regionLength);
	ImGui::Text("Chunk Position: %s", glm::to_string(chunkPosition).c_str());
	ImGui::Text("Region Position: %s", glm::to_string(regionPosition).c_str());

	ImGui::PushItemWidth(100);
	ImGui::SliderFloat("World Spherical Falloff", &sphericalFalloff, 0.0f, 0.1f);
	ImGui::SliderInt("Render Distance", &renderDistance, 2, 30);
	ImGui::Text("Region Count: %i", regions.size());
	
	int chunkCount = 0;
	for (auto& region : regions) chunkCount += region.second->GetChunkCount();
	ImGui::Text("Chunk Count: %i", chunkCount);

	const float timeScaled = metaData.timeOfDay / dayDuration * 24;
	const int timeHour = timeScaled;
	const int timeMinutes = (timeScaled - timeHour) * 60;
	ImGui::Text("Time Of Day: %i:%i", timeHour, timeMinutes);


	if(ImGui::Button("Clean Regions"))
	{
		SaveChunks();
		regions.clear();
	}

	if (ImGui::Button("Leave World"))
	{
		Exit();
	}

	ImGui::End();
}

void World::PlaceBlockFromPositionInDirection(const glm::vec3& position, const glm::vec3& direction, const float maxDistance, CubeID blockToPlace, bool shouldReplaceBlock)
{
	glm::vec3 currentPosition = glm::floor(position);
	const glm::vec3 endPosition = glm::floor(position + direction * maxDistance);
	const glm::vec3 tStep = glm::sign(direction);

	glm::vec3 tNear;
	tNear.x = (tStep.x >= 0) ? currentPosition.x + 1 - position.x : position.x - currentPosition.x;
	tNear.y = (tStep.y >= 0) ? currentPosition.y + 1 - position.y : position.y - currentPosition.y;
	tNear.z = (tStep.z >= 0) ? currentPosition.z + 1 - position.z : position.z - currentPosition.z;
	tNear += tStep * std::numeric_limits<float>::min();

	glm::vec3 tMax = tNear / direction;
	const glm::vec3 tDelta = glm::vec3(1) / direction;

	glm::ivec3 blockPosition;
	glm::vec3 prevPos;

	const float distance = abs(endPosition.x - currentPosition.x) + abs(endPosition.y - currentPosition.y) + abs(endPosition.z - currentPosition.z);
	for (int i = 0; i <= distance; ++i)
	{
		prevPos = currentPosition;
		if (abs(tMax.x) < abs(tMax.y) && abs(tMax.x) < abs(tMax.z)) {
			tMax.x += tDelta.x;
			currentPosition.x += tStep.x;
		}
		else if (abs(tMax.y) < abs(tMax.x) && abs(tMax.y) < abs(tMax.z)) {
			tMax.y += tDelta.y;
			currentPosition.y += tStep.y;
		}
		else {
			tMax.z += tDelta.z;
			currentPosition.z += tStep.z;
		}
	
		if (SharedPtr<Chunk> chunk = GetOrCreateChunkFromWorldPosition(currentPosition, false))
		{
			GetBlockWorldPosition(currentPosition, blockPosition);
			if (chunk->GetBlock(blockPosition).GetID() != Air)
			{
				if (shouldReplaceBlock)
				{
					PlaceBlock(chunk, blockPosition, blockToPlace);
				}
				else
				{
					if (glm::distance(prevPos, position) > 1.f)
					{
						SharedPtr<Chunk> prevChunk = GetOrCreateChunkFromWorldPosition(prevPos, true);
						glm::ivec3 prevBlockPosition;
						GetBlockWorldPosition(prevPos, prevBlockPosition);
						PlaceBlock(prevChunk, prevBlockPosition, blockToPlace);
					}
				}
				return;
			}
		}
	}
}

void World::PlaceBlockAtPosition(const glm::ivec3& position, const CubeID cubeId, bool shouldReplicate)
{
	const glm::ivec3 chunkPositionUnwrapped = RoundDownToMultiple(position, CHUNK_LENGTH);
	const glm::ivec3 chunkPosition = TranslateIntoWrappedWorld(chunkPositionUnwrapped) / CHUNK_LENGTH;
	const glm::ivec3 blockPosition = position - chunkPositionUnwrapped;

	if(SharedPtr<Chunk> chunk = FindChunk(chunkPosition))
	{
		PlaceBlock(chunk, blockPosition, cubeId, shouldReplicate);
	}
	else
	{
		PlaceBlockInUnloadedChunk(chunkPosition, blockPosition, cubeId, IsHostWorld());
	}
}

void World::PlaceBlockInUnloadedChunk(const glm::ivec3& chunkPosition, const glm::ivec3& blockPosition, const CubeID cubeId, bool shouldReplicate)
{
	//	#TODO this should be moved into a static chunk function.
	//  also note that we are appending the save data, and allowing our next load to clean up any duplicates,
	//	this potentially wastes perf when loading, but loading isn't on the main thread, so prioritising main thread perf is the current aim.
	//	This could break depending on the implementation of new blocks. I.e. if we call any construction code upon loading blocks which will be overwritten.

	const String filename = GetChunkDataFile(chunkPosition);
	std::fstream file = std::fstream(filename, std::ios::app | std::ios::binary);
	file.write((char*)&ChunkBlockData(blockPosition, cubeId), sizeof(ChunkBlockData));
	file.close();

	if (shouldReplicate)
	{
		ReplicatePlaceBlock(chunkPosition * CHUNK_LENGTH, blockPosition, cubeId);
	}
}

void World::PlaceBlock(SharedPtr<Chunk> chunk, const glm::ivec3& blockPosition, const CubeID cubeId, bool shouldReplicate)
{
	if (IsHostWorld() || !shouldReplicate)
	{
		chunk->SetBlockAtPosition(blockPosition, cubeId);
	}

	if (shouldReplicate)
	{
		ReplicatePlaceBlock(chunk->GetWorldPosition(), blockPosition, cubeId);
	}
}

void World::ReplicatePlaceBlock(const glm::ivec3& chunkWorldPosition, const glm::ivec3& blockPosition, const CubeID cubeId)
{
	const glm::ivec3 position = chunkWorldPosition + blockPosition;
	std::ostringstream os;
	os << "pb " << std::to_string(position.x) << " " << std::to_string(position.y) << " " << std::to_string(position.z) << " " << std::to_string(cubeId);

	for (auto netPlayer : networkPlayers)
	{
		SendPacket(os.str(), netPlayer->GetNetPeer());
	}
}


SharedPtr<Chunk> World::GetOrCreateChunkFromWorldPosition(const glm::vec3& position, bool createIfNull)
{
	const glm::ivec3 flooredPos = glm::floor(position);
	const glm::ivec3 regionWorldPosition = RoundDownToMultiple(flooredPos, metaData.regionLength * CHUNK_LENGTH) * glm::ivec3(1, 0, 1);
	const glm::ivec3 blockWorldPos = flooredPos - regionWorldPosition;
	const glm::ivec3 chunkWorldPos = RoundDownToMultiple(blockWorldPos, CHUNK_LENGTH);
	const glm::ivec3 regionPosition = regionWorldPosition / (metaData.regionLength * CHUNK_LENGTH);
	const glm::ivec3 chunkPos = chunkWorldPos / CHUNK_LENGTH;
	SharedPtr<ChunkRegion> region = GetOrCreateRegion(regionPosition);
	SharedPtr<Chunk> chunk = region->GetChunk(chunkPos);
	if (!chunk && createIfNull)
	{
		chunk = MakeShared<Chunk>(chunkPos);
		region->InsertChunk(chunk);
	}

	return chunk;
}

void World::GetBlockWorldPosition(const glm::vec3& position, glm::ivec3& outBlockWorldPosition)
{
	const glm::ivec3 flooredPos = glm::floor(position);
	const glm::ivec3 regionWorldPosition = RoundDownToMultiple(flooredPos, metaData.regionLength * CHUNK_LENGTH) * glm::ivec3(1, 0, 1);
	const glm::ivec3 blockWorldPos = flooredPos - regionWorldPosition;
	const glm::ivec3 chunkWorldPos = RoundDownToMultiple(blockWorldPos, CHUNK_LENGTH);
	outBlockWorldPosition = blockWorldPos - chunkWorldPos;
}

bool World::TryLoadMetaData()
{
	_mkdir(GetWorldDirectory().c_str());
	_mkdir(GetWorldChunkDataDirectory().c_str());

	std::ifstream file(GetWorldMetadataDirectory().c_str(), std::ios::in | std::ios::binary);
	if (file.is_open())
	{
		file.read((char*)&metaData, sizeof(WorldMetaData));

		DPrintf("Loaded %s, seed: %i, region length: %i\n", worldName.c_str(), metaData.seed, metaData.regionLength);
		return true;
	}

	return false;
}

void World::SaveMetaData()
{
	std::fstream file = std::fstream(GetWorldMetadataDirectory().c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
	if (file.is_open())
	{
		file.write((char*)&metaData, sizeof(WorldMetaData));
	}
}

void World::SaveChunks()
{
	auto chunkSet = std::set<SharedPtr<Chunk>>();
	for (auto& region : regions)
	{
		for (auto it : region.second->chunks)
		{
			if (SharedPtr<Chunk> chunk = it.second)
			{
				if (chunk->ShouldTrySave())
				{
					chunkSet.insert(chunk);
				}
			}
		}
	}

	for (auto chunk : chunkSet)
	{
		chunkManager->RequestTask(MakeShared<SaveJob>(chunk));
	}
}

void World::Exit()
{
	if (netClient) enet_host_destroy(netClient);
	enet_deinitialize();

	SaveMetaData();
	SaveChunks();
	chunkManager->Exit();

	auto& gameManager = GameManager::Instance();
	gameManager.ClearWorld();
	gameManager.CreateWorldLoader();
}

void World::UpdateNetClient()
{
	if (netClient)
	{
		ENetEvent event;
	
		while (enet_host_service(netClient, &event, 0) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
			{
				DPrintf("A new client connected from %x:%u.\n", event.peer->address.host, event.peer->address.port);
				BroadcastPlayerConnection(++lastPlayerUniqueId);
				networkPlayers.push_back(MakeShared<NetworkPlayer>(event.peer, lastPlayerUniqueId));
				SaveMetaData();
				SendPacket((enet_uint8 *)&metaData, sizeof(WorldMetaData), event.peer, 0);
				InformPlayerConnections(networkPlayers.back());
				break;
			}
			case ENET_EVENT_TYPE_RECEIVE:
				HandlePacket(event);
				break;
	
			case ENET_EVENT_TYPE_DISCONNECT:
				DPrintf("%s disconnected.\n", event.peer->data);
				for(auto& netPlayer : networkPlayers)
				{
					if (netPlayer->GetNetPeer() == event.peer)
					{
						BroadcastPlayerLeave(netPlayer->GetUniqueID());
						OnPlayerDisconnect(netPlayer);
						break;
					}
				}
				event.peer->data = NULL;
			}
		}
	}
}

void World::OnPlayerDisconnect(const SharedPtr<NetworkPlayer> disconnectee)
{
	networkPlayers.erase(std::remove(networkPlayers.begin(), networkPlayers.end(), disconnectee), networkPlayers.end());
}

void World::InformPlayerConnections(SharedPtr<NetworkPlayer> informee)
{
	for (auto& netPlayer : networkPlayers)
	{
		if (netPlayer != informee)
		{
			std::vector<enet_uint8> data = { 'p', 'j', ' ' };
			const auto uniqueIdPtr = (enet_uint8*)&netPlayer->GetUniqueID();
			data.insert(data.end(), uniqueIdPtr, uniqueIdPtr + sizeof(enet_uint32));
			SendPacket(&data.front(), data.size(), informee->GetNetPeer(), 0);
		}
	}
}

void World::BroadcastPlayerConnection(const enet_uint32& uniqueId)
{
	std::vector<enet_uint8> data = { 'p', 'j', ' ' };
	const auto uniqueIdPtr = (enet_uint8*)&uniqueId;
	data.insert(data.end(), uniqueIdPtr, uniqueIdPtr + sizeof(enet_uint32));
	for (auto& netPlayer : networkPlayers)
	{
		SendPacket(&data.front(), data.size(), netPlayer->GetNetPeer(), 0);
	}
}

void World::BroadcastPlayerLeave(const enet_uint32& uniqueId)
{
	std::vector<enet_uint8> data = { 'p', 'l', ' ' };
	const auto uniqueIdPtr = (enet_uint8*)&uniqueId;
	data.insert(data.end(), uniqueIdPtr, uniqueIdPtr + sizeof(enet_uint32));
	for (auto& netPlayer : networkPlayers)
	{
		SendPacket(&data.front(), data.size(), netPlayer->GetNetPeer(), 0);
	}
}

void World::HandlePacket(ENetEvent& event)
{
	DPrintf("A packet of length %u containing %s was received from %s on channel %u.\n", event.packet->dataLength, event.packet->data, event.peer->data, event.channelID);
	
	String delimiter(" ");
	const String data((char*)event.packet->data);
	
	std::vector<String> words;
	
	int pos = 0;
	String dataDelimSearch = data;
	String token;
	while ((pos = dataDelimSearch.find(delimiter)) != std::string::npos) {
		token = dataDelimSearch.substr(0, pos);
		words.push_back(token);
		dataDelimSearch.erase(0, pos + delimiter.length());
	}
	words.push_back(dataDelimSearch);
	
	switch (event.channelID)
	{
	case 0: 
	{
		if (HandlePlaceBlockPacket(words, event)) break;
		if (HandlePlayerPositionPacket(words, event)) break;
		if (HandlePlayerJoinPacket(words, event)) break;
		if (HandlePlayerLeavePacket(words, event)) break;
	}

	case 1:
	{
		if (HandleRequestChunkPacket(words, event)) break;
		if (HandleGenerateChunkPacket(words, event)) break;
	}
	}
	
	enet_packet_destroy(event.packet);
}

bool World::HandlePlayerLeavePacket(const std::vector<String>& words, const ENetEvent& event)
{
	if (words[0] == "pl")
	{
		if (words.size() > 1)
		{
			const int offset = 3;
			const auto packetData = event.packet->data + offset;
			const auto playerUniqueId = *(enet_uint32*)packetData;
			for(auto& networkPlayer : networkPlayers)
			{
				if (networkPlayer->GetUniqueID() == playerUniqueId)
				{
					OnPlayerDisconnect(networkPlayer);
					break;
				}
			}
		}
		else {
			DPrint("pl needs more params.");
		}

		return true;
	}
}


bool World::HandlePlayerJoinPacket(const std::vector<String>& words, const ENetEvent& event)
{
	if(words[0] == "pj") {
		if(words.size() > 1)
		{
			const int offset = 3;
			const auto packetData = event.packet->data + offset;
			const auto playerUniqueId = (enet_uint32*)packetData;
			networkPlayers.push_back(MakeShared<NetworkPlayer>(nullptr, *playerUniqueId));
		}
		else {
			DPrint("pj needs more params.");
		}

		return true;
	}

	return false;
}

bool World::HandleGenerateChunkPacket(const std::vector<String>& words, const ENetEvent& event)
{
	if (words[0] == "gc") { //Generate Chunk
		if (words.size() > 3) {
			const int offset = words[0].length() + words[1].length() + words[2].length() + words[3].length() + 4;
			const auto packetData = event.packet->data + offset;
			auto blockData = (ChunkBlockData*)packetData;
			std::vector<ChunkBlockData> chunkData;
			for (int i = offset; i < event.packet->dataLength - 2; i += sizeof(ChunkBlockData)) {
				chunkData.push_back(*blockData);
				DPrintf("Receving block at position %s with id %i \n", glm::to_string(blockData->GetPosition()).c_str(), blockData->blockId);
				blockData++;
			}
			GenerateChunkFromPacket(glm::ivec3(std::stoi(words[1]), std::stoi(words[2]), std::stoi(words[3])), chunkData);
		}
		else {
			DPrint("gc needs more params.");
		}

		return true;
	}

	return false;
}

bool World::HandleRequestChunkPacket(const std::vector<String>& words, const ENetEvent& event)
{
	if (words[0] == "rc") { //Request Chunk
		if (words.size() > 3) {
			SendChunkData(glm::ivec3(std::stoi(words[1]), std::stoi(words[2]), std::stoi(words[3])), event.peer);
		}
		else {
			DPrint("rc needs more params.");
		}

		return true;
	}

	return false;
}


bool World::HandlePlaceBlockPacket(const std::vector<String>& words, const ENetEvent& event)
{
	if (words[0] == "pb") { //Place Block
		if (words.size() > 4) {
			PlaceBlockAtPosition(glm::ivec3(std::stoi(words[1]), std::stoi(words[2]), std::stoi(words[3])), CubeID(std::atoi(words[4].c_str())), IsHostWorld());
		}
		else {
			DPrint("pb needs more params.");
		}

		return true;
	}

	return false;
}

bool World::HandlePlayerPositionPacket(const std::vector<String>& words, const ENetEvent& event)
{
	if (words[0] == "pp") //Player Position
	{
		const auto uniqueIdData = event.packet->data + 3;
		const auto uniqueId = *(enet_uint32*)uniqueIdData;
		const auto transformData = event.packet->data + 4 + sizeof(enet_uint32);
		const auto playerTransform = (Transform<glm::vec3>*)transformData;

		UpdateNetworkPlayerTransform(event.peer, uniqueId, *playerTransform);
		if (IsHostWorld())
		{
			for (auto& netPlayer : networkPlayers)
			{
				if (netPlayer->GetNetPeer() == event.peer)
				{
					BroadcastPlayerPosition(netPlayer);
					break;
				}
			}
		}

		return true;
	}

	return false;
}

void World::SendChunkData(const glm::ivec3& chunkPosition, ENetPeer* peer)
{
	std::vector<enet_uint8> data;
	String command = "gc " + std::to_string(chunkPosition.x) + " " + std::to_string(chunkPosition.y) + " " + std::to_string(chunkPosition.z);
	data.insert(data.end(), command.begin(), command.end());

	auto foundChunk = FindChunk(chunkPosition);
	if (foundChunk && foundChunk->ShouldTrySave())
	{
		foundChunk->SaveChunkData();
	}

	const String filename = GetChunkDataFile(chunkPosition);
	std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary);

	if (file.is_open())
	{
		data.push_back(' ');

		const int startIndex = data.size();

		while (!file.eof())
		{
			ChunkBlockData blockData;
			file.read((char*)&blockData, sizeof(ChunkBlockData));
			const auto blockDataPtr = (enet_uint8*)&blockData;
			data.insert(data.end(), blockDataPtr, blockDataPtr + sizeof(ChunkBlockData));
			DPrintf("Sending block at position %s with id %i \n", glm::to_string(blockData.GetPosition()).c_str(), blockData.blockId);
		}	

		file.close();

	}
	data.push_back(0);
	SendPacket(&data.front(), data.size(), peer, 1);
}

void World::GenerateChunkFromPacket(const glm::ivec3 chunkPosition, const std::vector<ChunkBlockData>& data)
{
	auto chunk = FindChunk(chunkPosition);
	if (chunk && !chunk->IsLoaded())
	{
		chunkManager->RequestTask(MakeShared<ClientGenerateJob>(chunk, data));
	}
}

void World::SendPacket(const String& data, ENetPeer* peer, const int channel)
{
	SendPacket((enet_uint8*)data.c_str(), data.length(), peer, channel);
}

void World::BroadcastPacket(const enet_uint8* data, const int dataSize, const int channel)
{
	for (auto& netPlayerForReceive : networkPlayers)
	{
		SendPacket(data, dataSize, netPlayerForReceive->GetNetPeer(), channel);
	}
}

void World::SendPacket(const enet_uint8* data, const int dataSize, ENetPeer* peer, const int channel)
{
	ENetPacket* packet = enet_packet_create(data, dataSize+1, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, channel, packet);
	enet_host_flush(netClient);
}

void World::SendLocalPlayerPosition()
{
	std::vector<enet_uint8> data = { 'p', 'p', ' ' };
	const enet_uint32 hostUniqueId = 0;
	const auto uniqueIdPtr = (enet_uint8*)&hostUniqueId;
	data.insert(data.end(), uniqueIdPtr, uniqueIdPtr + sizeof(enet_uint32));
	data.push_back(' ');
	const auto transformData = (enet_uint8*)&player->GetTransform();
	data.insert(data.end(), transformData, transformData + sizeof(Transform<glm::vec3>));
	BroadcastPacket(&data.front(), data.size(), 0);
}

void World::BroadcastPlayerPosition(SharedPtr<NetworkPlayer> netPlayer)
{
	std::vector<enet_uint8> data = { 'p', 'p', ' ' };
	const auto uniqueIdPtr = (enet_uint8*)&netPlayer->GetUniqueID();
	data.insert(data.end(), uniqueIdPtr, uniqueIdPtr + sizeof(enet_uint32));
	data.push_back(' ');
	const auto transformData = (enet_uint8*)&netPlayer->GetTransform();
	data.insert(data.end(), transformData, transformData + sizeof(Transform<glm::vec3>));

	for (auto& netPlayerToSend : networkPlayers)
	{
		if (netPlayerToSend != netPlayer)
		{
			SendPacket(&data.front(), data.size(), netPlayerToSend->GetNetPeer(), 0);
		}
	}
}

void World::UpdateNetworkPlayerTransform(const ENetPeer* sender, const uint32_t uniqueId, const Transform<glm::vec3>& transform)
{
	for (auto& netPlayer : networkPlayers)
	{
		if (netPlayer->GetNetPeer() == sender)
		{
			netPlayer->SetTransform(transform);
		}
	}
}