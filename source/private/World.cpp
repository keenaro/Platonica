#include "World.h"
#include "ChunkRegion.h"
#include "Player.h"
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
	chunkManager.MergeCompletedTasks(*this);
	player->Update(deltaTime);
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
