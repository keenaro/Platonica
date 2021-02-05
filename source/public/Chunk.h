#include "RenderObject.h"
#include "glm/vec3.hpp"
#include "Cube.h"

#define CHUNK_WIDTH 16

class Chunk : public RenderObject
{
public:
	Chunk(bool addToRenderList = true);

public:
	bool IsInsideChunk(const glm::vec3& position) const;

private:
	void GenerateBuffers();
	void GenerateChunkData();
	void UpdateFaces();
	void AddCubeAtPosition(const glm::ivec3& positionInsideChunk, const Cube& cube, std::vector<int32_t>& vertices, std::vector<unsigned int>& indices) const;
	glm::ivec3 GetCubePositionFromCubeVertex(int32_t inData) const;
	glm::ivec3 GetNormalFromCubeVertex(int32_t inData) const;
	glm::ivec3 GetVertexPositionFromCubeVertex(int32_t inData) const;
	int32_t GetCubeVertexData(const CubeID cubeID, const glm::ivec3& vertexPosition, const glm::ivec3& position, const glm::ivec3& normal) const;
	void SetVertexAttributePointer() override;
	void SetShaderUniformValues() override;
	void Draw() override;

private:
	bool dirty;
	glm::ivec3 position;
	Cube data[CHUNK_WIDTH][CHUNK_WIDTH][CHUNK_WIDTH];
};