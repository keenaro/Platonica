#include "World.h"
#include "ChunkRegion.h"
#include "Player.h"

World::World(int inRegionLength) : RenderObject(true), UpdateObject(true)
{
	regionLength = inRegionLength;
	shader = ShaderLibrary::GetShader(std::string("WorldCubes"));
	player = MakeShared<Player>();
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
	player->Update(deltaTime);
}

void World::GenerateChunk(glm::ivec3 pos)
{	
	SharedPtr<Chunk> chunk = MakeShared<Chunk>(pos);
	GetOrCreateRegion(pos)->InsertChunk(chunk);
}

SharedPtr<ChunkRegion> World::GetOrCreateRegion(glm::ivec3 pos)
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
