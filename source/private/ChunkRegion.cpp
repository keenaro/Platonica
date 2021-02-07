#include "ChunkRegion.h"
#include "World.h"

ChunkRegion::ChunkRegion(glm::ivec3& inPosition) : RenderObject(false), Position(inPosition)
{
	position = inPosition;
	shader = World::Instance().GetShader();
}

void ChunkRegion::Draw()
{
	RenderObject::Draw();

	IterateMap3(chunks)
	{
		if (SharedPtr<Chunk> chunk = mapIt.second)
		{
			chunk->Draw();
		}
	}
}

void ChunkRegion::InsertChunk(SharedPtr<Chunk> chunk)
{
	glm::ivec3 chunkPos = chunk->GetPosition();
	chunks[chunkPos.x][chunkPos.y][chunkPos.z] = chunk;
}

void ChunkRegion::SetShaderUniformValues()
{
	shader->SetIVector3("RegionPosition", position);
}


bool ChunkRegion::IsInsideRegion(glm::vec3& inPosition) const
{
	const int regionLength = World::Instance().GetRegionLength();
	return (inPosition.x > position.x && inPosition.x < position.x + regionLength * CHUNK_LENGTH && 
			inPosition.y > position.y && inPosition.y < position.y + regionLength * CHUNK_LENGTH && 
			inPosition.z > position.z && inPosition.z < position.z + regionLength * CHUNK_LENGTH);
}