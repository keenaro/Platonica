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

	void InsertChunk(SharedPtr<Chunk> chunk);

	bool IsInsideRegion(glm::vec3& inPosition) const;
	bool IsInsideRegion(glm::ivec3& inPosition) const { IsInsideRegion(glm::vec3(inPosition)); };

private:
	SharedMap3<Chunk> chunks;
};

