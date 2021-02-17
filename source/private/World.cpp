#include "World.h"
#include "ChunkRegion.h"
#include "Player.h"
#include <algorithm>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

World::World(int inRegionLength) : RenderObject(true), UpdateObject(true)
{
	regionLength = inRegionLength;
	shader = ShaderLibrary::GetShader(std::string("WorldCubes"));
	player = MakeShared<Player>();
	LoadTextureAtlas();
	chunkManager.Start();
}

void World::Draw() 
{
	currentChunkBufferCount = 0;

	RenderObject::Draw();
	IterateMap3(regions)
	{
		if (SharedPtr<ChunkRegion> region = mapIt.second)
		{
			region->Draw();
		}	
	}
}

void World::Update(float deltaTime)
{
	player->Update(deltaTime);
	TryRequestChunks();	
}

void World::TryRequestChunks()
{
	const int renderDistance = 5;
	const glm::ivec3 playerPositionChunkSpace = player->GetPosition() * (1.f / CHUNK_LENGTH);
	std::vector<glm::ivec3> newChunkPositionsInRange;
	for (int z = -renderDistance; z < renderDistance; z++)
	{
		for (int y = -renderDistance; y < renderDistance; y++)
		{
			for (int x = -renderDistance; x < renderDistance; x++)
			{
				glm::ivec3& chunkPosition = playerPositionChunkSpace + glm::ivec3(x, y, z);
				glm::ivec3& regionPosition = glm::ivec3(0, 0, 0); // HARD CODED FIX PLS

				const SharedPtr<ChunkRegion> region = GetOrCreateRegion(regionPosition);
				SharedPtr<Chunk> chunk = region->TryCreateChunk(chunkPosition);
				if (chunk)
				{
					chunkManager.RequestTask(chunk);
				}
			}
		}
	}
}

SharedPtr<ChunkRegion> World::GetOrCreateRegion(glm::ivec3& pos)
{
	if (!regions[pos.x][pos.y][pos.z])
	{
		SharedPtr<ChunkRegion> region = MakeShared<ChunkRegion>(pos);
		regions[pos.x][pos.y][pos.z] = region;
	}

	return regions[pos.x][pos.y][pos.z];
}

void World::SetShaderUniformValues()
{
	const glm::mat4 worldXform = glm::mat4(1);
	const glm::mat4 viewWorldXform = player->GetViewXForm() * worldXform;

	shader->SetVector3("CameraPosition", player->GetPosition());
	shader->SetMatrix4("ViewWorldXform", viewWorldXform);
	shader->SetMatrix4("ProjectionXform", player->GetProjectionXForm());
}

void World::LoadTextureAtlas()
{
	unsigned int texture;
	glGenTextures(0, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	int width, height, nrChannels;
	unsigned char* data = stbi_load("textures/atlas.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		DPrint("Failed to load texture");
	}
	stbi_image_free(data);
}

bool World::TryIncrementChunkGenBufferCount()
{
	const bool shouldIncrement = currentChunkBufferCount < maxChunkBufferGensPerTick;
	currentChunkBufferCount += shouldIncrement;
	return currentChunkBufferCount < maxChunkBufferGensPerTick;
}