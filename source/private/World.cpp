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
	const glm::ivec3& playerChunkPosition = playerWorldPosition / CHUNK_LENGTH;

	const int renderRadius = renderDistance / 2;
	for (int z = -renderRadius; z < renderRadius; z++)
	{
		for (int y = -renderRadius; y < renderRadius; y++)
		{
			for (int x = -renderRadius; x < renderRadius; x++)
			{
				const glm::ivec3& unwrappedChunkPositionChunkSpace = playerChunkPosition + glm::ivec3(x, y, z);
				const glm::ivec3& chunkPositionChunkSpace = TranslateIntoWrappedWorld(unwrappedChunkPositionChunkSpace);
				const glm::ivec3& regionPositionChunkSpace = RoundDownToMultiple(unwrappedChunkPositionChunkSpace, regionLength * CHUNK_LENGTH) * glm::ivec3(1,0,1);
				const glm::vec3& chunkPositionWorldSpace = (regionPositionChunkSpace + chunkPositionChunkSpace) * CHUNK_LENGTH;

				if (glm::distance(glm::vec3(playerWorldPosition), chunkPositionWorldSpace) < GetOffloadDistance())
				{
					const glm::ivec3& regionPositionRegionSpace = regionPositionChunkSpace / regionLength;
					const SharedPtr<ChunkRegion> region = GetOrCreateRegion(regionPositionRegionSpace);
					if (!region->GetChunk(chunkPositionChunkSpace))
					{
						SharedPtr<Chunk> chunk = GetChunk(chunkPositionChunkSpace);
						if (!chunk)
						{
							DPrintf("Requesting chunk %s\n", glm::to_string(chunkPositionChunkSpace).c_str());
							chunk = MakeShared<Chunk>(chunkPositionChunkSpace);
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

	const glm::vec3 playerAbsolutePostion = player->GetPosition();
	ImGui::Text("Player Absolute Position: %s", glm::to_string(playerAbsolutePostion).c_str());
	ImGui::Text("Player Wrapped Position: %s", glm::to_string(TranslateIntoWrappedWorld(playerAbsolutePostion/regionLength)*regionLength).c_str());
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