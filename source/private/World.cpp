#include "World.h"
#include "ChunkRegion.h"
#include "Player.h"
#include <algorithm>
#include "PerlinNoise.h"
#include "Defs.h"
#include "MathFunctions.h"

World::World(int inRegionLength) : RenderObject(true), UpdateObject(true)
{
	regionLength = inRegionLength;
	shader = ShaderLibrary::GetShader(std::string("WorldCubes"));
	player = MakeShared<Player>();
	perlin = MakeShared<PerlinNoise>();
	LoadTexture("textures/atlas.png");
	chunkManager.Start();
}

void World::Draw() 
{
	RenderObject::Draw();
	DrawRegions();
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
	UpdateRegions(deltaTime);
	player->Update(deltaTime);
	TryRequestChunks();	
	UpdateGUI();
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
				const glm::ivec3& regionPositionWorldSpace = RoundDownToMultiple(unwrappedChunkWorldPosition, regionLength * CHUNK_LENGTH) * glm::ivec3(1, 0, 1);
				const glm::ivec3& regionPosition = regionPositionWorldSpace / (CHUNK_LENGTH * regionLength);

				if (glm::distance(glm::vec3(playerWorldPosition), glm::vec3(unwrappedChunkWorldPosition)) < GetOffloadDistance())
				{
					const SharedPtr<ChunkRegion> region = GetOrCreateRegion(regionPosition);
					if (!region->GetChunk(chunkPosition))
					{
						SharedPtr<Chunk> chunk = GetChunk(chunkPosition);
						if (!chunk)
						{
							DPrintf("Requesting chunk at Region(%s) Chunk(%s)\n", glm::to_string(regionPosition).c_str(), glm::to_string(chunkPosition).c_str());
							chunk = MakeShared<Chunk>(chunkPosition);
							chunkManager.RequestTask(chunk);
						}

						region->InsertChunk(chunk);
					}
				}
			}
		}
	}
}

SharedPtr<Chunk> World::GetChunk(const glm::ivec3& chunkPosition) const
{
	for(auto& region : regions)
	{
		if(SharedPtr<Chunk> chunk = region.second->GetChunk(chunkPosition))
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
}

int World::TranslateIntoWrappedWorld(int value) const
{
	const int regionWorldLength = regionLength * CHUNK_LENGTH;
	return (value + regionWorldLength * (abs(value / regionWorldLength) + 1)) % regionWorldLength;
}

glm::ivec3 World::TranslateIntoWrappedWorld(const glm::ivec3& vec3ToTranslate) const
{
	return glm::ivec3(TranslateIntoWrappedWorld(vec3ToTranslate.x), vec3ToTranslate.y, TranslateIntoWrappedWorld(vec3ToTranslate.z));
}

void World::UpdateGUI()
{
	ImGui::Begin("World");

	const glm::ivec3& playerWorldPosition = player->GetPosition();
	const glm::ivec3& playerChunkPosition = RoundDownToMultiple(playerWorldPosition, CHUNK_LENGTH) / CHUNK_LENGTH;
	const glm::ivec3& unwrappedChunkPosition = playerChunkPosition;
	const glm::ivec3& unwrappedChunkWorldPosition = unwrappedChunkPosition * CHUNK_LENGTH;
	const glm::ivec3& chunkWorldPosition = TranslateIntoWrappedWorld(unwrappedChunkWorldPosition);
	const glm::ivec3& chunkPosition = chunkWorldPosition / CHUNK_LENGTH;
	const glm::ivec3& regionPositionWorldSpace = RoundDownToMultiple(unwrappedChunkWorldPosition, regionLength * CHUNK_LENGTH) * glm::ivec3(1, 0, 1);
	const glm::ivec3& regionPosition = regionPositionWorldSpace / (CHUNK_LENGTH * regionLength);
	ImGui::Text("Chunk Position: %s", glm::to_string(chunkPosition).c_str());
	ImGui::Text("Region Position: %s", glm::to_string(regionPosition).c_str());

	ImGui::Text("Player World Position: %s", glm::to_string(playerWorldPosition).c_str());
	ImGui::Text("Player Wrapped Position: %s", glm::to_string(TranslateIntoWrappedWorld(playerWorldPosition /regionLength)*regionLength).c_str());
	ImGui::PushItemWidth(100);
	ImGui::SliderFloat("World Spherical Falloff", &sphericalFalloff, 0.0f, 0.5f);
	ImGui::SliderInt("Render Distance", &renderDistance, 2, 30);
	ImGui::Text("Region Count: %i", regions.size());
	
	int chunkCount = 0;
	for (auto& region : regions) chunkCount += region.second->GetChunkCount();
	ImGui::Text("Chunk Count: %i", chunkCount);

	if(ImGui::Button("Clean Regions"))
	{
		regions.clear();
	}




	ImGui::End();
}

void World::PlaceBlockInPlayerSight()
{
	const int regionLen = GetRegionLength();

	const glm::vec3 startPosition = player->GetPosition();
	const glm::vec3 endPosition = startPosition + player->GetDirection() * player->GetReach();
	const glm::vec3 deltaPos = endPosition - startPosition;

	float step = glm::max(glm::max(abs(deltaPos.x), abs(deltaPos.y)), abs(deltaPos.z));
	const glm::vec3 stepAmount = deltaPos / step;

	const auto GetStepInfo = [this, &startPosition, &stepAmount](glm::ivec3& outBlockPosition, int step, bool createChunkIfNull = false) -> SharedPtr<Chunk>
	{
		const glm::vec3 stepPos = startPosition + stepAmount * step;
		const glm::ivec3 flooredPos = glm::floor(stepPos);
		const glm::ivec3 regionWorldPosition = RoundDownToMultiple(flooredPos, regionLength * CHUNK_LENGTH) * glm::ivec3(1,0,1);
		const glm::ivec3 blockWorldPos = flooredPos - regionWorldPosition;
		const glm::ivec3 chunkWorldPos = RoundDownToMultiple(blockWorldPos, CHUNK_LENGTH);
		const glm::ivec3 blockPos = blockWorldPos - chunkWorldPos;
		const glm::ivec3 regionPosition = regionWorldPosition / (regionLength * CHUNK_LENGTH);
		const glm::ivec3 chunkPos = chunkWorldPos / CHUNK_LENGTH;
		outBlockPosition = blockPos;
		SharedPtr<ChunkRegion> region = GetOrCreateRegion(regionPosition);
		SharedPtr<Chunk> chunk = region->GetChunk(chunkPos);
		if (!chunk && createChunkIfNull)
		{
			chunk = MakeShared<Chunk>(chunkPos);
			region->InsertChunk(chunk);
		}

		return chunk;
	};

	glm::ivec3 blockPosition;
	for (int stepI = 1; stepI <= step; stepI++) {
		if(SharedPtr<Chunk> chunk = GetStepInfo(blockPosition, stepI))
		{
			if (chunk->GetBlock(blockPosition).GetID() != Air)
			{
				glm::ivec3 prevBlockPosition;
				if (stepI > 1)
				{
					SharedPtr<Chunk> prevChunk = GetStepInfo(prevBlockPosition, stepI - 1, true);
					prevChunk->SetBlockAtPosition(prevBlockPosition, CubeID::Stone);
				}

				return;
			}
		}
	}
}