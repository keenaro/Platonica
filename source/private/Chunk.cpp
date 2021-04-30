#include "Chunk.h"
#include "CubeDefs.h"
#include "World.h"
#include <random>
#include "Player.h"
#include "Defs.h"
#include <fstream>
#include "GameManager.h"

Chunk::Chunk(const glm::ivec3& inPosition) : VertexRenderObject(false), Position(inPosition)
{
	dirty = true;
}

bool Chunk::ShouldDraw(const glm::ivec3& chunkRegionWorldPosition) const
{
	const SharedPtr<World> world = GameManager::Instance().GetWorld();
	const float renderDistance = world->GetRenderDistance();
	const SharedPtr<Player> player = world->GetPlayer();
	const glm::vec3& cameraPosition = player->GetPosition();
	const glm::vec3& cameraDirection = player->GetDirection();
	const glm::vec3& chunkWorldPosition = GetWorldCentrePosition() + chunkRegionWorldPosition;
	const glm::vec3& infrontOffset = cameraDirection * CHUNK_LENGTH;

	const bool IsInfront = glm::fastNormalizeDot(chunkWorldPosition - cameraPosition + infrontOffset, cameraDirection) > 0.25f;
	const bool IsWithinRenderDistance = glm::distance(cameraPosition, chunkWorldPosition) < renderDistance;

	return loaded && (dirty || VertexRenderObject::ShouldDraw()) && IsInfront && IsWithinRenderDistance;
}

void Chunk::Draw()
{
	if (dirty)
	{
		GenerateBuffers();
	}

	VertexRenderObject::Draw();
}

void Chunk::SetShaderUniformValues()
{
	GameManager::Instance().GetWorld()->GetShader()->SetIVector3("ChunkPosition", GetWorldPosition());
}

void Chunk::GenerateBuffers()
{
	std::vector<int32_t> ChunkVertices;
	std::vector<unsigned int> ChunkIndices;

	for (int z = 0; z < CHUNK_LENGTH; z++)
	{
		for (int y = 0; y < CHUNK_LENGTH; y++)
		{
			for (int x = 0; x < CHUNK_LENGTH; x++)
			{
				const Cube& cube = data[x][y][z];

				if (cube.CanSee())
				{
					AddCubeAtPosition(glm::ivec3(x, y, z), cube, ChunkVertices, ChunkIndices);
				}
			}
		}
	}

	dirty = false;
	BindData(ChunkVertices, ChunkIndices);
}

void Chunk::UpdateAllFaces()
{
	for (int z = 0; z < CHUNK_LENGTH; z++)
	{
		for (int y = 0; y < CHUNK_LENGTH; y++)
		{
			for (int x = 0; x < CHUNK_LENGTH; x++)
			{
				UpdateCubeFaces(glm::ivec3(x, y, z));
			}
		}
	}
}

void Chunk::UpdateCubeFaces(const glm::ivec3& cubePosition)
{
	Cube& cube = data[cubePosition.x][cubePosition.y][cubePosition.z];
	cube.SetFaceData(CubeFace::All);

	const SharedPtr<CubeDefs> cubeDefs = GameManager::Instance().GetCubeDefs();
	const SharedPtr<World> world = GameManager::Instance().GetWorld();
	const glm::ivec3 cp = position * CHUNK_LENGTH;

	const auto shouldCull = [this, &world, &cubeDefs, &cubePosition, &cp](glm::ivec3& offset) 
	{
		const auto offsetPosition = cubePosition + offset;
		if (IsPositionInsideChunk(offsetPosition))
			return GetBlock(offsetPosition).CanSee();

		//#TODO This code can be used for neighboring chunk culling, but requires a rework in order to make the chunk container thread safe.
		//else if (auto chunk = world->FindChunk(position + offset))
		//{
		//	auto blockPosition = (offsetPosition + CHUNK_LENGTH * (abs(offsetPosition / CHUNK_LENGTH) + 1)) % CHUNK_LENGTH;
		//	return chunk->GetBlock(blockPosition).CanSee();
		//}
		//return cubeDefs->ShouldAffectCull(GetCubeIdAtPosition(cp + offsetPosition));

		return false;
	};

	if (shouldCull(glm::ivec3(0,-1,0)))
		cube.CullFace(CubeFace::Bottom);

	if (shouldCull(glm::ivec3(0, 1, 0)))
		cube.CullFace(CubeFace::Top);

	if (shouldCull(glm::ivec3(-1, 0, 0)))
		cube.CullFace(CubeFace::Right);

	if (shouldCull(glm::ivec3(1, 0, 0)))
		cube.CullFace(CubeFace::Left);

	if (shouldCull(glm::ivec3(0, 0, -1)))
		cube.CullFace(CubeFace::Back);

	if (shouldCull(glm::ivec3(0, 0, 1)))
		cube.CullFace(CubeFace::Front);
}

void Chunk::AddCubeAtPosition(const glm::ivec3& positionInsideChunk, const Cube& cube, std::vector<int32_t>& vertices, std::vector<unsigned int>& indices) const
{
	const CubeFace faces = cube.GetFaceData();
	int indicesStartingIndex = vertices.size();

	if (faces & CubeFace::Back)
	{
		vertices.push_back(GetCubeVertexData(cube.GetID(), glm::ivec3(1, 1, 0), positionInsideChunk, glm::ivec3(0, 0, -1)));
		vertices.push_back(GetCubeVertexData(cube.GetID(), glm::ivec3(1, 0, 0), positionInsideChunk, glm::ivec3(0, 0, -1)));
		vertices.push_back(GetCubeVertexData(cube.GetID(), glm::ivec3(0, 0, 0), positionInsideChunk, glm::ivec3(0, 0, -1)));
		vertices.push_back(GetCubeVertexData(cube.GetID(), glm::ivec3(0, 1, 0), positionInsideChunk, glm::ivec3(0, 0, -1)));
	}

	if (faces & CubeFace::Top)
	{
		vertices.push_back(GetCubeVertexData(cube.GetID(), glm::ivec3(1, 1, 1), positionInsideChunk, glm::ivec3(0, 1, 0)));
		vertices.push_back(GetCubeVertexData(cube.GetID(), glm::ivec3(1, 1, 0), positionInsideChunk, glm::ivec3(0, 1, 0)));
		vertices.push_back(GetCubeVertexData(cube.GetID(), glm::ivec3(0, 1, 0), positionInsideChunk, glm::ivec3(0, 1, 0)));
		vertices.push_back(GetCubeVertexData(cube.GetID(), glm::ivec3(0, 1, 1), positionInsideChunk, glm::ivec3(0, 1, 0)));
	}

	if (faces & CubeFace::Front)
	{
		vertices.push_back(GetCubeVertexData(cube.GetID(), glm::ivec3(0, 1, 1), positionInsideChunk, glm::ivec3(0, 0, 1)));
		vertices.push_back(GetCubeVertexData(cube.GetID(), glm::ivec3(0, 0, 1), positionInsideChunk, glm::ivec3(0, 0, 1)));
		vertices.push_back(GetCubeVertexData(cube.GetID(), glm::ivec3(1, 0, 1), positionInsideChunk, glm::ivec3(0, 0, 1)));
		vertices.push_back(GetCubeVertexData(cube.GetID(), glm::ivec3(1, 1, 1), positionInsideChunk, glm::ivec3(0, 0, 1)));
	}

	if (faces & CubeFace::Bottom)
	{
		vertices.push_back(GetCubeVertexData(cube.GetID(), glm::ivec3(0, 0, 1), positionInsideChunk, glm::ivec3(0, -1, 0)));
		vertices.push_back(GetCubeVertexData(cube.GetID(), glm::ivec3(0, 0, 0), positionInsideChunk, glm::ivec3(0, -1, 0)));
		vertices.push_back(GetCubeVertexData(cube.GetID(), glm::ivec3(1, 0, 0), positionInsideChunk, glm::ivec3(0, -1, 0)));
		vertices.push_back(GetCubeVertexData(cube.GetID(), glm::ivec3(1, 0, 1), positionInsideChunk, glm::ivec3(0, -1, 0)));
	}

	if (faces & CubeFace::Right)
	{
		vertices.push_back(GetCubeVertexData(cube.GetID(), glm::ivec3(0, 1, 0), positionInsideChunk, glm::ivec3(-1, 0, 0)));
		vertices.push_back(GetCubeVertexData(cube.GetID(), glm::ivec3(0, 0, 0), positionInsideChunk, glm::ivec3(-1, 0, 0)));
		vertices.push_back(GetCubeVertexData(cube.GetID(), glm::ivec3(0, 0, 1), positionInsideChunk, glm::ivec3(-1, 0, 0)));
		vertices.push_back(GetCubeVertexData(cube.GetID(), glm::ivec3(0, 1, 1), positionInsideChunk, glm::ivec3(-1, 0, 0)));
	}

	if (faces & CubeFace::Left)
	{
		vertices.push_back(GetCubeVertexData(cube.GetID(), glm::ivec3(1, 1, 1), positionInsideChunk, glm::ivec3(1, 0, 0)));
		vertices.push_back(GetCubeVertexData(cube.GetID(), glm::ivec3(1, 0, 1), positionInsideChunk, glm::ivec3(1, 0, 0)));
		vertices.push_back(GetCubeVertexData(cube.GetID(), glm::ivec3(1, 0, 0), positionInsideChunk, glm::ivec3(1, 0, 0)));
		vertices.push_back(GetCubeVertexData(cube.GetID(), glm::ivec3(1, 1, 0), positionInsideChunk, glm::ivec3(1, 0, 0)));
	}

	const int numOfFaces = cube.GetNumberOfFaces();

	for (int i = 0; i < numOfFaces; i++)
	{
		indices.push_back(indicesStartingIndex + 0);
		indices.push_back(indicesStartingIndex + 1);
		indices.push_back(indicesStartingIndex + 3);
		indices.push_back(indicesStartingIndex + 1);
		indices.push_back(indicesStartingIndex + 2);
		indices.push_back(indicesStartingIndex + 3);

		indicesStartingIndex += 4;
	}
}

glm::ivec3 Chunk::GetCubePositionFromCubeVertex(int32_t inData) const
{
	const int x = (inData >> 24) & 15;
	const int y = (inData >> 20) & 15;
	const int z = (inData >> 16) & 15;

	return glm::ivec3(x, y, z);
}

glm::ivec3 Chunk::GetNormalFromCubeVertex(int32_t inData) const
{
	const int sign = 1 - (inData >> 31) * 2;
	const int x = ((inData >> 30) & 1) * sign;
	const int y = ((inData >> 29) & 1) * sign;
	const int z = ((inData >> 28) & 1) * sign;

	return glm::ivec3(x, y, z);
}

glm::ivec3 Chunk::GetVertexPositionFromCubeVertex(int32_t inData) const
{
	const int x = (inData >> 15) & 1;
	const int y = (inData >> 14) & 1;
	const int z = (inData >> 13) & 1;

	return glm::ivec3(x, y, z);
}

int32_t Chunk::GetCubeVertexData(const CubeID cubeID, const glm::ivec3& vertexPosition, const glm::ivec3& position, const glm::ivec3& normal) const
{
	//Normals 
	//Assume x, y or z will be 1 or -1, therefore it should be 0 if false.
	const int32_t sign = (normal.x + normal.y + normal.z < 0) << 31;
	const int32_t normalX = (normal.x & 1) << 30;
	const int32_t normalY = (normal.y & 1) << 29;
	const int32_t normalZ = (normal.z & 1) << 28;

	//Cube Position
	//Assume x, y or z will be under 16. 0000 <- 15
	const int32_t posX = (int32_t(position.x) & 31) << 24;
	const int32_t posY = (int32_t(position.y) & 31) << 20;
	const int32_t posZ = (int32_t(position.z) & 31) << 16;

	//Vertex Position
	const int32_t vposX = (vertexPosition.x & 1) << 15;
	const int32_t vposY = (vertexPosition.y & 1) << 14;
	const int32_t vposZ = (vertexPosition.z & 1) << 13;

	//Texture Coordinates
	const SharedPtr<CubeDefs> cubeDefs = GameManager::Instance().GetCubeDefs();
	const CubeDef& cubeDef = cubeDefs->GetCubeDef(cubeID);
	const int32_t texCoord = cubeDef.GetTextureCoordinatesFromNormal(normal);

	const int32_t data = sign + normalX + normalY + normalZ + posX + posY + posZ + vposX + vposY + vposZ + texCoord;

	return data;
}

void Chunk::GenerateUnmodifiedChunkData()
{
	const glm::vec3 WSChunkPosition = position * CHUNK_LENGTH;
	const SharedPtr<World> world = GameManager::Instance().GetWorld();
	const unsigned int baseHeight = 20;
	const float inclineMultiplier = 40.0f;
	const int regionLength = world->GetRegionLength();
	const float frequency = 2.f;
	const float minFrequency = glm::min((float)regionLength, frequency > 0 ? 1 << int(frequency) : frequency);

	std::list<glm::ivec3> treeBases;

	for (int z = 0; z < CHUNK_LENGTH; z++)
	{
		for (int x = 0; x < CHUNK_LENGTH; x++)
		{
			const glm::vec3 wrappedXZ = world->TranslateIntoWrappedWorld(glm::vec3(x, 0, z) + WSChunkPosition);
			const int terrainHeight = GetTerrainHeightAtWrappedPosition(wrappedXZ);

			if (WSChunkPosition.y < terrainHeight)
			{
				for (int y = 0; y < CHUNK_LENGTH; y++)
				{
					const glm::vec3 wrappedPosition = world->TranslateIntoWrappedWorld(glm::vec3(x, y, z) + WSChunkPosition);

					if (wrappedPosition.y >= terrainHeight || IsCaveAtWrappedPosition(wrappedPosition, terrainHeight))
					{
						data[x][y][z].SetID(Air);
					}
					else
					{
						data[x][y][z].SetID(wrappedPosition.y + 1 == terrainHeight ? Grass : wrappedPosition.y + 5 > terrainHeight ? Dirt : Stone);
					}
				}
			}

			if (terrainHeight >= WSChunkPosition.y && terrainHeight < WSChunkPosition.y + CHUNK_LENGTH)
			{
				if (IsTreeBaseAtWrappedPosition(wrappedXZ))
				{
					treeBases.push_back(glm::ivec3(x, terrainHeight - (int)WSChunkPosition.y, z));
				}
			}
		}
	}

	for (auto& treePos : treeBases)
	{
		GrowTreeAtBlockPosition(treePos);
	}
}

void Chunk::GenerateChunkData(const ChunkBlockData* extraChunkData, const int extraChunkLength)
{
	GenerateUnmodifiedChunkData();

	if (GameManager::Instance().GetWorld()->IsHostWorld())
	{
		TryLoadChunkData();
	}

	if (extraChunkData)
	{
		const ChunkBlockData* chunkData = extraChunkData;
		for (int pos = 0; pos < extraChunkLength; pos++)
		{
			const auto blockPos = chunkData->GetPosition();
			data[blockPos.x][blockPos.y][blockPos.z] = chunkData->blockId;
			chunkData++;
		}

	}

	UpdateAllFaces();
	loaded = true;
}

void Chunk::GrowTreeAtBlockPosition(const glm::ivec3& position)
{
	int treeRadius = 1;
	int treeHeight = 5;
	int treeLeavesThickness = 3;

	if(position.y + treeHeight > CHUNK_LENGTH || position.x - treeRadius < 0 || position.x + treeRadius > CHUNK_LENGTH-1 || position.z - treeRadius < 0 || position.z + treeRadius > CHUNK_LENGTH - 1)
	{
		return; //Hack: We are too close to the chunk boundary don't bother.
	}

	for(int h = 0; h < treeHeight; h++)
	{
		if(h >= treeHeight - treeLeavesThickness)
		{
			for(int lx = -treeRadius; lx <= treeRadius; lx++)
			{
				for (int lz = -treeRadius; lz <= treeRadius; lz++)
				{
					if (!lx || abs(lx) != abs(lz) || h != treeHeight-1)
					{
						data[position.x + lx][position.y + h][position.z + lz].SetID(Leaves);
					}
				}
			}
		}
		else
		{
			data[position.x][position.y + h][position.z].SetID(Wood);
		}
	}
}

bool Chunk::TryLoadChunkData()
{
	const String filename = GameManager::Instance().GetWorld()->GetChunkDataFile(position);
	std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary);
	if (file.is_open())
	{
		while (!file.eof())
		{
			ChunkBlockData blockData;
			file.read((char*)&blockData, sizeof(ChunkBlockData));
			const auto blockPos = blockData.GetPosition();
			data[blockPos.x][blockPos.y][blockPos.z] = blockData.blockId;
		}

		return true;
	}

	return false;
}

CubeID Chunk::GetBlockFromSave(const glm::ivec3& chunkPosition, const glm::ivec3& blockPosition)
{
	const String filename = GameManager::Instance().GetWorld()->GetChunkDataFile(position);
	std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary);
	if (file.is_open())
	{
		while (!file.eof())
		{
			ChunkBlockData blockData;
			file.read((char*)&blockData, sizeof(ChunkBlockData));
			if (blockPosition == blockData.GetPosition()) return blockData.blockId;
		}
	}

	return GetCubeIdAtPosition(chunkPosition * CHUNK_LENGTH + blockPosition);
}

void Chunk::SaveChunkData()
{
	if (GameManager::Instance().GetWorld()->IsHostWorld())
	{
		const String filename = GameManager::Instance().GetWorld()->GetChunkDataFile(position);
		std::fstream file;

		const glm::vec3 WSChunkPosition = position * CHUNK_LENGTH;

		Chunk unmodifiedChunk(position);
		unmodifiedChunk.GenerateUnmodifiedChunkData();

		for (uint16_t z = 0; z < CHUNK_LENGTH; z++)
		{
			for (uint16_t x = 0; x < CHUNK_LENGTH; x++)
			{
				for (uint16_t y = 0; y < CHUNK_LENGTH; y++)
				{
					if (unmodifiedChunk.data[x][y][z].GetID() != data[x][y][z].GetID())
					{
						if (!file.is_open()) {
							file = std::fstream(filename, std::ios::out | std::ios::trunc | std::ios::binary);
						}
						file.write((char*)&ChunkBlockData(glm::ivec3(x,y,z), data[x][y][z].GetID()), sizeof(ChunkBlockData));
					}
				}
			}
		}

		if (file.is_open())
		{
			file.close();
		}
	}
}

int Chunk::GetTerrainHeightAtWrappedPosition(const glm::vec3& position)
{
	const SharedPtr<World> world = GameManager::Instance().GetWorld();
	const unsigned int baseHeight = 20;
	const float inclineMultiplier = 40.0f;
	const int regionLength = world->GetRegionLength();
	const float frequency = 2.f;
	const float minFrequency = glm::min((float)regionLength, frequency > 0 ? 1 << int(frequency) : frequency);

	const float pNoise = PerlinNoise::PNoise(glm::vec3(position.x / CHUNK_LENGTH, position.z / CHUNK_LENGTH, 0.0f) / minFrequency, glm::vec3(regionLength, regionLength, minFrequency) / minFrequency);
	return pNoise * inclineMultiplier + baseHeight;
}

bool Chunk::IsCaveAtWrappedPosition(const glm::vec3& position, int terrainHeight)
{
	const int caveThreshold = 10;

	const SharedPtr<World> world = GameManager::Instance().GetWorld();
	const int regionLength = world->GetRegionLength();
	const float caveInclineMultiplier = terrainHeight + abs(position.y) * 0.01f;
	const float testNoise = PerlinNoise::PNoise(glm::vec3(position.x / CHUNK_LENGTH, position.z / CHUNK_LENGTH, position.y / CHUNK_LENGTH), glm::ivec3(regionLength, regionLength, 10000.0f));
	const int tnoise = testNoise * caveInclineMultiplier;
	return (tnoise > caveThreshold);
}

bool Chunk::IsTreeBaseAtWrappedPosition(const glm::vec3& position)
{
	const float treeFreq = 1.1f;
	const float treeNoise = abs(PerlinNoise::PNoise(glm::vec3(position.x / treeFreq, position.z / treeFreq, 0.0f)));
	return (treeNoise > 0.5f);
}

CubeID Chunk::GetCubeIdAtPosition(const glm::vec3& cubePosition)
{
	const SharedPtr<World> world = GameManager::Instance().GetWorld();
	const glm::vec3 wrappedPosition = world->TranslateIntoWrappedWorld(cubePosition);

	CubeID id = Air;

	const int terrainHeight = GetTerrainHeightAtWrappedPosition(wrappedPosition);

	if (wrappedPosition.y < terrainHeight)
	{
		if (IsCaveAtWrappedPosition(wrappedPosition, terrainHeight)) {
			id = Air;
		}
		else {
			id = wrappedPosition.y + 1 == terrainHeight ? Grass : wrappedPosition.y + 5 > terrainHeight ? Dirt : Stone;
		}
	}

	return id;
}

void Chunk::SetBlockAtPosition(const glm::ivec3& blockPosition, CubeID newBlock)
{
	DPrintf("Placed block at %s\n", glm::to_string(blockPosition + position).c_str());
	data[blockPosition.x][blockPosition.y][blockPosition.z] = newBlock;
	UpdateCubeFaces(blockPosition);

	const SharedPtr<World> world = GameManager::Instance().GetWorld();

	//#TODO UPDATE BLOCKS ON ADJACENT CHUNKS

	if (IsPositionInsideChunk(blockPosition + glm::ivec3(-1, 0, 0))) {
		UpdateCubeFaces(blockPosition + glm::ivec3(-1, 0, 0));
	}
	else if(auto chunk = world->FindChunk(position + glm::ivec3(-1, 0, 0))) {
		chunk->UpdateCubeFaces((blockPosition + glm::ivec3(-1, 0, 0)) % CHUNK_LENGTH);
	}

	if (IsPositionInsideChunk(blockPosition + glm::ivec3(1, 0, 0))) {
		UpdateCubeFaces(blockPosition + glm::ivec3(1, 0, 0));
	}
	else if (auto chunk = world->FindChunk(position + glm::ivec3(1, 0, 0))) {
		chunk->UpdateCubeFaces((blockPosition + glm::ivec3(1, 0, 0)) % CHUNK_LENGTH);
	}

	if (IsPositionInsideChunk(blockPosition + glm::ivec3(0, -1, 0))) {
		UpdateCubeFaces(blockPosition + glm::ivec3(0, -1, 0));
	}
	else if (auto chunk = world->FindChunk(position + glm::ivec3(0, -1, 0))) {
		chunk->UpdateCubeFaces((blockPosition + glm::ivec3(0, -1, 0)) % CHUNK_LENGTH);
	}

	if (IsPositionInsideChunk(blockPosition + glm::ivec3(0, 1, 0))) {
		UpdateCubeFaces(blockPosition + glm::ivec3(0, 1, 0));
	}
	else if (auto chunk = world->FindChunk(position + glm::ivec3(0, 1, 0))) {
		chunk->UpdateCubeFaces((blockPosition + glm::ivec3(0, 1, 0)) % CHUNK_LENGTH);
	}

	if (IsPositionInsideChunk(blockPosition + glm::ivec3(0, 0, -1))) {
		UpdateCubeFaces(blockPosition + glm::ivec3(0, 0, -1));
	}
	else if (auto chunk = world->FindChunk(position + glm::ivec3(0, 0, -1))) {
		chunk->UpdateCubeFaces((blockPosition + glm::ivec3(0, 0, -1)) % CHUNK_LENGTH);
	}

	if (IsPositionInsideChunk(blockPosition + glm::ivec3(0, 0, 1))) {
		UpdateCubeFaces(blockPosition + glm::ivec3(0, 0, 1));
	}
	else if (auto chunk = world->FindChunk(position + glm::ivec3(0, 0, 1))) {
		chunk->UpdateCubeFaces((blockPosition + glm::ivec3(0, 0, 1)) % CHUNK_LENGTH);
	}

	dirty = true;
	shouldTrySave = true;
}

bool Chunk::IsPositionInsideChunk(const glm::ivec3& position) const 
{
	return position.x > -1 && position.x < CHUNK_LENGTH && position.y > -1 && position.y < CHUNK_LENGTH && position.z > -1 && position.z < CHUNK_LENGTH;
}

void Chunk::SetVertexAttributePointer()
{
	glVertexAttribIPointer(0, 1, GL_INT, sizeof(int32_t), (void*)0);
}
