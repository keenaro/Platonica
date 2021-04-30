#pragma once
#include "RenderObject.h"
#include "Cube.h"
#include "Transform.h"

#define CHUNK_LENGTH 16

struct ChunkBlockData
{
public:
	uint16_t blockPosition;
	CubeID blockId;

	ChunkBlockData() {};
	ChunkBlockData(const glm::ivec3& position, const CubeID id) : blockId(id) { blockPosition = uint16_t((position.x << 8) + (position.y << 4) + position.z); };
	glm::ivec3 GetPosition() const { return glm::ivec3((blockPosition >> 8) & 15, (blockPosition >> 4) & 15, blockPosition & 15); };
};

class Chunk : public VertexRenderObject, public Position<glm::ivec3>
{
public:
	Chunk(const glm::ivec3& inPosition);

public:
	void Draw() override;
	bool ShouldDraw(const glm::ivec3& chunkRegionWorldPosition) const;

	void SaveChunkData();
	void GenerateUnmodifiedChunkData();
	void GenerateChunkData(const ChunkBlockData* extraChunkData = nullptr, const int extraChunkLength = 0);
	glm::ivec3 GetWorldPosition() const { return position * CHUNK_LENGTH; };
	glm::ivec3 GetWorldCentrePosition() const { return position * CHUNK_LENGTH + glm::ivec3(CHUNK_LENGTH / 2); }

	void SetBlockAtPosition(const glm::ivec3& blockPosition, CubeID newBlock);
	const Cube& GetBlock(const glm::ivec3& position) const { return data[position.x][position.y][position.z]; }

	bool IsLoaded() const { return loaded; };

	static CubeID GetCubeIdAtPosition(const glm::vec3& position);
	static int GetTerrainHeightAtWrappedPosition(const glm::vec3& position);
	static bool IsCaveAtWrappedPosition(const glm::vec3& position, int terrainHeight);
	static bool IsTreeBaseAtWrappedPosition(const glm::vec3& position);

	bool ShouldTrySave() const { return shouldTrySave; };

private:
	void GenerateBuffers();
	void UpdateAllFaces();
	bool UpdateCubeFaces(const glm::ivec3& cubePosition);
	void AddCubeAtPosition(const glm::ivec3& positionInsideChunk, const Cube& cube, std::vector<int32_t>& vertices, std::vector<unsigned int>& indices) const;
	glm::ivec3 GetCubePositionFromCubeVertex(int32_t inData) const;
	glm::ivec3 GetNormalFromCubeVertex(int32_t inData) const;
	glm::ivec3 GetVertexPositionFromCubeVertex(int32_t inData) const;
	int32_t GetCubeVertexData(const CubeID cubeID, const glm::ivec3& vertexPosition, const glm::ivec3& position, const glm::ivec3& normal) const;
	void SetVertexAttributePointer() override;
	void SetShaderUniformValues() override;
	bool IsPositionInsideChunk(const glm::ivec3& position) const;
	bool TryLoadChunkData();
	CubeID GetBlockFromSave(const glm::ivec3& chunkPosition, const glm::ivec3& blockPosition);

	void GrowTreeAtBlockPosition(const glm::ivec3& position);

private:
	bool dirty = true;
	Cube data[CHUNK_LENGTH][CHUNK_LENGTH][CHUNK_LENGTH];
	bool loaded = false;
	bool shouldTrySave = false;
};