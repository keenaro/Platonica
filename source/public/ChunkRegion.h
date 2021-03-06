#pragma once
#include "Chunk.h"
#include "Defs.h"
#include "UpdateObject.h"
#include "Transform.h"

class ChunkRegion : public RenderObject, public Position<glm::ivec3>, public UpdateObject
{
public:
	ChunkRegion(const glm::ivec3& inPosition = glm::ivec3(0));

	void Draw() override;
	void Update(float deltaTime) override;

	void InsertChunk(SharedPtr<Chunk> chunk);

	bool IsInsideRegion(glm::vec3& inPosition) const;
	bool IsInsideRegion(glm::ivec3& inPosition) const { IsInsideRegion(glm::vec3(inPosition)); };

	SharedPtr<Chunk> GetChunk(const glm::ivec3& chunkPosition);

	glm::ivec3 GetWorldPosition() const;
	glm::ivec3 GetChunkWorldPosition(SharedPtr<Chunk> chunk) const { return chunk->GetWorldPosition() + GetWorldPosition(); }

	int GetChunkCount() const { return chunks.size(); }

private:
	void RemoveOutOfDistanceChunks();
	void DrawChunks() const;
	void SetShaderUniformValues() override;

private:
	SharedIVec3Map<Chunk> chunks;
};

