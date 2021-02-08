#include "Chunk.h"
#include "CubeDefs.h"
#include "World.h"

Chunk::Chunk(glm::ivec3& inPosition) : VertexRenderObject(false), Position(inPosition)
{
	position = inPosition;
	dirty = true;
	shader = World::Instance().GetShader();
	GenerateChunkData();
	GenerateBuffers();
}

void Chunk::Draw()
{
	if(dirty)
	{
		GenerateBuffers();
	}

	VertexRenderObject::Draw();
}

void Chunk::SetShaderUniformValues()
{
	shader->SetIVector3("ChunkPosition", position);
}

void Chunk::GenerateBuffers()
{
	std::vector<int32_t> ChunkVertices;
	std::vector<unsigned int> ChunkIndices;

	//24 ints per cube, Chunk is 16x16x16 cubes.
	ChunkVertices.reserve(24 * CHUNK_LENGTH * CHUNK_LENGTH * CHUNK_LENGTH);
	
	//36 indices per cube, Chunk is 16x16x16 cubes.
	ChunkIndices.reserve(36 * CHUNK_LENGTH * CHUNK_LENGTH * CHUNK_LENGTH);
	
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
				Cube& cube = data[x][y][z];
				cube.SetFaceData(CubeFace::All);

				if (y > 0 && data[x][y-1][z].CanSee()) 
					cube.CullFace(CubeFace::Bottom);

				if (y < CHUNK_LENGTH - 1 && data[x][y+1][z].CanSee())
					cube.CullFace(CubeFace::Top);

				if (x > 0 && data[x-1][y][z].CanSee()) 
					cube.CullFace(CubeFace::Right);

				if (x < CHUNK_LENGTH - 1 && data[x+1][y][z].CanSee())
					cube.CullFace(CubeFace::Left);

				if (z > 0 && data[x][y][z-1].CanSee()) 
					cube.CullFace(CubeFace::Back);
				
				if (z < CHUNK_LENGTH - 1 && data[x][y][z+1].CanSee())
					cube.CullFace(CubeFace::Front);		
			}
		}
	}
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

	//Quick way of getting how many faces there are. #todo move CubeFace into a class and have relevant functions in there.
	const int numOfFaces = bool(faces & CubeFace::Front) +
		bool(faces & CubeFace::Back) +
		bool(faces & CubeFace::Right) +
		bool(faces & CubeFace::Left) +
		bool(faces & CubeFace::Bottom) +
		bool(faces & CubeFace::Top);

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
		for (int y = 0; y < CHUNK_LENGTH; y++)
		{
			for (int x = 0; x < CHUNK_LENGTH; x++)
			{
				data[x][y][z].SetID(CubeID::Grass);
			}
		}
	}

	UpdateAllFaces();
}

bool Chunk::IsInsideChunk(const glm::vec3& inPos) const
{
	return (inPos.x > position.x && inPos.x < position.x + CHUNK_LENGTH &&
		inPos.y > position.y && inPos.y < position.y + CHUNK_LENGTH &&
		inPos.z > position.z && inPos.z < position.z + CHUNK_LENGTH);
}

void Chunk::SetVertexAttributePointer()
{
	glVertexAttribIPointer(0, 1, GL_INT, sizeof(int32_t), (void*)0);
}
