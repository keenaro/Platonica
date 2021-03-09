#include "Chunk.h"
#include "CubeDefs.h"
#include "World.h"
#include <random>
#include "Player.h"
#include "Defs.h"

Chunk::Chunk(const glm::ivec3& inPosition) : VertexRenderObject(false), Position(inPosition)
{
	dirty = true;
	shader = World::Instance().GetShader();
}

bool Chunk::ShouldDraw(const glm::ivec3& chunkRegionWorldPosition) const
{
	const World& world = World::Instance();
	const float renderDistance = world.GetRenderDistance();
	const SharedPtr<Player> player = world.GetPlayer();
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
	shader->SetIVector3("ChunkPosition", GetWorldPosition());
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

void Chunk::UpdateCubeFaces(const glm::ivec3& position)
{
	Cube& cube = data[position.x][position.y][position.z];
	cube.SetFaceData(CubeFace::All);

	if (position.y > 0 && data[position.x][position.y - 1][position.z].CanSee())
		cube.CullFace(CubeFace::Bottom);

	if (position.y < CHUNK_LENGTH - 1 && data[position.x][position.y + 1][position.z].CanSee())
		cube.CullFace(CubeFace::Top);

	if (position.x > 0 && data[position.x - 1][position.y][position.z].CanSee())
		cube.CullFace(CubeFace::Right);

	if (position.x < CHUNK_LENGTH - 1 && data[position.x + 1][position.y][position.z].CanSee())
		cube.CullFace(CubeFace::Left);

	if (position.z > 0 && data[position.x][position.y][position.z - 1].CanSee())
		cube.CullFace(CubeFace::Back);

	if (position.z < CHUNK_LENGTH - 1 && data[position.x][position.y][position.z + 1].CanSee())
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
	const CubeDefs& cubeDefs = CubeDefs::Instance();
	const CubeDef& cubeDef = cubeDefs.GetCubeDef(cubeID);
	const int32_t texCoord = cubeDef.GetTextureCoordinatesFromNormal(normal);

	const int32_t data = sign + normalX + normalY + normalZ + posX + posY + posZ + vposX + vposY + vposZ + texCoord;

	return data;
}

void Chunk::GenerateChunkData()
{ 
	for (int z = 0; z < CHUNK_LENGTH; z++)
	{
		for (int x = 0; x < CHUNK_LENGTH; x++)
		{
			World& world = World::Instance();
			const int regionLength = world.GetRegionLength();

			const glm::vec3 WSChunkPosition = position * CHUNK_LENGTH;

			const unsigned int baseHeight = 20;
			const float inclineMultiplier = 60.0f;

			if (const SharedPtr<PerlinNoise> perlinGenerator = world.perlin)
			{
				const glm::vec3 noisePosition = glm::vec3(x + WSChunkPosition.x, z + WSChunkPosition.y, 0) / CHUNK_LENGTH;

				const int frequency = 2;
				const float minFrequency = glm::min(regionLength, frequency * 2);

				const float pNoise = perlinGenerator->PNoise(glm::vec3((x + WSChunkPosition.x) / CHUNK_LENGTH, (z + WSChunkPosition.z) / CHUNK_LENGTH, 0.0f) / minFrequency, glm::vec3(regionLength, regionLength, minFrequency) / minFrequency);
				const int noise = pNoise * inclineMultiplier + baseHeight;

				for (int y = 0; y < CHUNK_LENGTH; y++)
				{
					const int caveFreq = 16;
					const float caveInclineMultiplier = position.y;
					const int caveThreshold = 1;
					const float testNoise = perlinGenerator->PNoise(glm::vec3((x + WSChunkPosition.x) / CHUNK_LENGTH, (z + WSChunkPosition.z) / CHUNK_LENGTH, (y + WSChunkPosition.y) / CHUNK_LENGTH), glm::ivec3(regionLength, regionLength, 10000.0f));
					const int tnoise = testNoise * caveInclineMultiplier;

					CubeID id = Air;
					if (WSChunkPosition.y + y < noise)
					{
						id = WSChunkPosition.y + y + 1 == noise ? Grass : WSChunkPosition.y + y + 5 > noise ? Dirt : Stone;

						if (WSChunkPosition.y + y < noise - 5 && tnoise > caveThreshold) id = Air;
					}

					data[x][y][z].SetID(id);
				}
			}
		}
	}

	UpdateAllFaces();
	loaded = true;
}

void Chunk::SetBlockAtPosition(const glm::ivec3& position, CubeID newBlock)
{
	data[position.x][position.y][position.z] = newBlock;
	UpdateCubeFaces(position);

	if(IsPositionInsideChunk(position + glm::ivec3(-1,0,0))) {
		UpdateCubeFaces(position + glm::ivec3(-1, 0, 0));
	}
	if (IsPositionInsideChunk(position + glm::ivec3(1, 0, 0))) {
		UpdateCubeFaces(position + glm::ivec3(1, 0, 0));
	}
	if (IsPositionInsideChunk(position + glm::ivec3(0, -1, 0))) {
		UpdateCubeFaces(position + glm::ivec3(0, -1, 0));
	}
	if (IsPositionInsideChunk(position + glm::ivec3(0, 1, 0))) {
		UpdateCubeFaces(position + glm::ivec3(0, 1, 0));
	}
	if (IsPositionInsideChunk(position + glm::ivec3(0, 0, -1))) {
		UpdateCubeFaces(position + glm::ivec3(0, 0, -1));
	}
	if (IsPositionInsideChunk(position + glm::ivec3(0, 0, 1))) {
		UpdateCubeFaces(position + glm::ivec3(0, 0, 1));
	}

	dirty = true;
}

bool Chunk::IsPositionInsideChunk(const glm::ivec3& position) const 
{
	return position.x >= 0 && position.x < CHUNK_LENGTH&& position.y >= 0 && position.y < CHUNK_LENGTH&& position.z >= 0 && position.z < CHUNK_LENGTH;
}

void Chunk::SetVertexAttributePointer()
{
	glVertexAttribIPointer(0, 1, GL_INT, sizeof(int32_t), (void*)0);
}
