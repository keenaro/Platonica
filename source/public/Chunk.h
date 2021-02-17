#pragma once
#include "RenderObject.h"
#include "Cube.h"
#include "Transform.h"

#define CHUNK_LENGTH 16

class Chunk : public VertexRenderObject, public Position<glm::ivec3>
{
public:
	Chunk(glm::ivec3& inPosition);

public:
	bool IsInsideChunk(const glm::vec3& position) const;
	void Draw() override;
	bool ShouldDraw() const override;

	void GenerateChunkData();

	static int maxChunkBufferGensPerTick;
	static int currentChunkBufferCount;

private:
	bool GenerateBuffers();
	void UpdateAllFaces();
	void AddCubeAtPosition(const glm::ivec3& positionInsideChunk, const Cube& cube, std::vector<int32_t>& vertices, std::vector<unsigned int>& indices) const;
	glm::ivec3 GetCubePositionFromCubeVertex(int32_t inData) const;
	glm::ivec3 GetNormalFromCubeVertex(int32_t inData) const;
	glm::ivec3 GetVertexPositionFromCubeVertex(int32_t inData) const;
	int32_t GetCubeVertexData(const CubeID cubeID, const glm::ivec3& vertexPosition, const glm::ivec3& position, const glm::ivec3& normal) const;
	void SetVertexAttributePointer() override;
	void SetShaderUniformValues() override;

private:
	bool dirty = true;
	Cube data[CHUNK_LENGTH][CHUNK_LENGTH][CHUNK_LENGTH];
	bool loaded = false;

};