#include "ChunkRegion.h"
#include "World.h"
#include "Player.h"

ChunkRegion::ChunkRegion(const glm::ivec3& inPosition) : RenderObject(false), Position(inPosition), UpdateObject(false)
{
	shader = World::Instance().GetShader();
}

void ChunkRegion::Draw()
{
	RenderObject::Draw();
	DrawChunks();
}

void ChunkRegion::DrawChunks() const
{
	const glm::ivec3& regionWorldPosition = GetWorldPosition();

	for (auto& it : chunks)
	{
		if (SharedPtr<Chunk> chunk = it.second)
		{
			if (chunk->ShouldDraw(regionWorldPosition))
			{
				chunk->Draw();
			}
		}
	}
}

void ChunkRegion::Update(float deltaTime)
{
	if (World::Instance().DidPlayerMoveChunk())
	{
		RemoveOutOfDistanceChunks();
	}
}

glm::ivec3 ChunkRegion::GetWorldPosition() const 
{ 
	return position * World::Instance().GetRegionLength() * CHUNK_LENGTH;
}

void ChunkRegion::RemoveOutOfDistanceChunks()
{
	const World& world = World::Instance();
	const glm::vec3 playerPos = world.GetPlayer()->GetPosition();
	const int offloadDistance = world.GetOffloadDistance();

	for (auto& it = chunks.cbegin(); it != chunks.cend();)
	{
		if (!it->second || glm::distance(glm::vec3(GetChunkWorldPosition(it->second)), playerPos) > offloadDistance)
		{
			chunks.erase(it++);
		}
		else
		{
			++it;
		}
	}
}

void ChunkRegion::InsertChunk(SharedPtr<Chunk> chunk)
{
	chunks[chunk->GetPosition()] = chunk;
}

void ChunkRegion::SetShaderUniformValues()
{
	shader->SetIVector3("RegionPosition", GetWorldPosition());
}

bool ChunkRegion::IsInsideRegion(glm::vec3& inPosition) const
{
	const int regionLength = World::Instance().GetRegionLength();
	return (inPosition.x > position.x && inPosition.x < position.x + regionLength * CHUNK_LENGTH && 
			inPosition.y > position.y && inPosition.y < position.y + regionLength * CHUNK_LENGTH && 
			inPosition.z > position.z && inPosition.z < position.z + regionLength * CHUNK_LENGTH);
}

SharedPtr<Chunk> ChunkRegion::GetChunk(const glm::ivec3& chunkPosition)
{
	return chunks[chunkPosition];
}
