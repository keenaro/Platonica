#pragma once
#include "Chunk.h"
#include "Defs.h"
#include "UpdateObject.h"
#include "Transform.h"

class ChunkRegion : public RenderObject, public Position<glm::ivec3>, public UpdateObject
{
public:
	ChunkRegion(glm::ivec3& inPosition = glm::ivec3(0));

	void Draw() override;
	void Update(float deltaTime) override;
	void SetShaderUniformValues() override;

	SharedPtr<Chunk> ChunkRegion::TryCreateChunk(glm::ivec3& chunkPosition);

	bool IsInsideRegion(glm::vec3& inPosition) const;
	bool IsInsideRegion(glm::ivec3& inPosition) const { IsInsideRegion(glm::vec3(inPosition)); };

	SharedPtr<Chunk> GetChunk(glm::ivec3& chunkPosition);

	glm::ivec3 GetWorldPosition() const { return position * chunkRegionLength * CHUNK_LENGTH; }
	glm::ivec3 GetChunkWorldPosition(SharedPtr<Chunk> chunk) const { return chunk->GetWorldPosition() + GetWorldPosition(); }

private:
	void RemoveOutOfDistanceChunks();

private:
	SharedIVec3Map<Chunk> chunks;

	const int chunkRegionLength = 16;
};

