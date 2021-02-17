#pragma once
#include "Chunk.h"
#include "Defs.h"
#include "Transform.h"

class ChunkRegion : public RenderObject, public Position<glm::ivec3>
{
public:
	ChunkRegion(glm::ivec3& inPosition = glm::ivec3(0));

	void Draw() override;
	void SetShaderUniformValues() override;

	SharedPtr<Chunk> ChunkRegion::TryCreateChunk(glm::ivec3& chunkPosition);

	bool IsInsideRegion(glm::vec3& inPosition) const;
	bool IsInsideRegion(glm::ivec3& inPosition) const { IsInsideRegion(glm::vec3(inPosition)); };

	SharedPtr<Chunk> GetChunk(glm::ivec3& chunkPosition);

private:
	SharedMap3<Chunk> chunks;
};

