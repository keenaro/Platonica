#include "World.h"
#include "ChunkRegion.h"
#include "Player.h"
#include <algorithm>
#include "PerlinNoise.h"
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
	RenderObject::Draw();
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
	for (auto& it : regions)
	{
		if (SharedPtr<ChunkRegion> region = it.second)
		{
			region->Update(deltaTime);
		}
	}
	player->Update(deltaTime);
	TryRequestChunks();	
}

void World::TryRequestChunks()
{
	const glm::ivec3 playerPositionChunkSpace = player->GetPosition() / CHUNK_LENGTH;
	const int renderRadius = renderDistance / 2;
	for (int z = -renderRadius; z < renderRadius; z++)
	{
		for (int y = -renderRadius; y < renderRadius; y++)
		{
			for (int x = -renderRadius; x < renderRadius; x++)
			{
				glm::ivec3& chunkPosition = playerPositionChunkSpace + glm::ivec3(x, y, z);
				glm::ivec3& regionPosition = glm::ivec3(0, 0, 0); // HARD CODED FIX PLS

				if (glm::distance(glm::vec3(playerPositionChunkSpace), glm::vec3(regionPosition * regionLength + chunkPosition)) < GetOffloadDistance())
				{
					const SharedPtr<ChunkRegion> region = GetOrCreateRegion(regionPosition);
					SharedPtr<Chunk> chunk = region->TryCreateChunk(chunkPosition);
					if (chunk)
					{
						DPrintf("Requesting chunk %s\n", glm::to_string(chunkPosition).c_str());
						chunkManager.RequestTask(chunk);
					}
				}
			}
		}
	}
}

SharedPtr<ChunkRegion> World::GetOrCreateRegion(glm::ivec3& pos)
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