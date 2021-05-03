#pragma once
#include "RenderObject.h"
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"

class SquareObject : public VertexRenderObject
{
public:
	SquareObject::SquareObject(glm::vec2& size, bool addToRenderList = true) : VertexRenderObject(addToRenderList)
	{
		const std::vector<float> vertices
		{
			size.x, -size.y, 0.0f,
			 size.x,  size.y, 0.0f,
			-size.x,  size.y, 0.0f,
			-size.x, -size.y, 0.0f,
		};

		const std::vector<unsigned int> indices
		{
			0, 1, 3,
			1, 2, 3,
		};

		shader = ShaderLibrary::GetShader(std::string("Basic"));
		BindData(vertices, indices);
	}
};

class TriangleObject : public VertexRenderObject
{
public:
	TriangleObject::TriangleObject(bool addToRenderList = true) : VertexRenderObject(addToRenderList)
	{
		const std::vector<float> vertices
		{
			0.0f,  0.1f, 0.0f,
			-0.1f, -0.1f, 0.0f,
			0.1f, -0.1f, 0.0f,
		};

		const std::vector<unsigned int> indices
		{
			0, 1, 2,
		};

		shader = ShaderLibrary::GetShader(std::string("Basic"));
		BindData(vertices, indices);
	}
};

struct VertexStructure
{
public:
	VertexStructure(const glm::vec3& inPosition, const glm::vec3& inNormal, const glm::vec2& inTexCoord) : position(inPosition), normal(inNormal), texCoord(inTexCoord) {};
public:
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
};

class BoxObject : public VertexRenderObject
{
public:
	BoxObject::BoxObject(const glm::vec3& offset, const glm::vec3& size, const glm::vec2& frontTexCoord, const glm::vec2& backTexCoord, const glm::vec2& topTexCoord, const glm::vec2& bottomTexCoord, const glm::vec2& leftTexCoord, const glm::vec2& rightTexCoord, String& shaderName = String("Standard"), bool addToRenderList = true) : VertexRenderObject(addToRenderList)
	{
		const glm::vec3 halfSize = size / 2;

		std::vector<VertexStructure> vertices;

		//Back
		vertices.push_back(VertexStructure(offset + glm::vec3(halfSize.x, halfSize.y, -halfSize.z), glm::vec3(0, 0, -1), backTexCoord));
		vertices.push_back(VertexStructure(offset + glm::vec3(halfSize.x, -halfSize.y, -halfSize.z), glm::vec3(0, 0, -1), backTexCoord));
		vertices.push_back(VertexStructure(offset + glm::vec3(-halfSize.x, -halfSize.y, -halfSize.z), glm::vec3(0, 0, -1), backTexCoord));
		vertices.push_back(VertexStructure(offset + glm::vec3(-halfSize.x, halfSize.y, -halfSize.z), glm::vec3(0, 0, -1), backTexCoord));

		//Top
		vertices.push_back(VertexStructure(offset + glm::vec3(halfSize.x, halfSize.y, halfSize.z), glm::vec3(0, 1, 0), topTexCoord));
		vertices.push_back(VertexStructure(offset + glm::vec3(halfSize.x, halfSize.y, -halfSize.z), glm::vec3(0, 1, 0), topTexCoord));
		vertices.push_back(VertexStructure(offset + glm::vec3(-halfSize.x, halfSize.y, -halfSize.z), glm::vec3(0, 1, 0), topTexCoord));
		vertices.push_back(VertexStructure(offset + glm::vec3(-halfSize.x, halfSize.y, halfSize.z), glm::vec3(0, 1, 0), topTexCoord));

		//Front
		vertices.push_back(VertexStructure(offset + glm::vec3(-halfSize.x, halfSize.y, halfSize.z), glm::vec3(0, 0, 1), frontTexCoord));
		vertices.push_back(VertexStructure(offset + glm::vec3(-halfSize.x, -halfSize.y, halfSize.z), glm::vec3(0, 0, 1), frontTexCoord));
		vertices.push_back(VertexStructure(offset + glm::vec3(halfSize.x, -halfSize.y, halfSize.z), glm::vec3(0, 0, 1), frontTexCoord));
		vertices.push_back(VertexStructure(offset + glm::vec3(halfSize.x, halfSize.y, halfSize.z), glm::vec3(0, 0, 1), frontTexCoord));

		//Bottom
		vertices.push_back(VertexStructure(offset + glm::vec3(-halfSize.x, -halfSize.y, halfSize.z), glm::vec3(0, -1, 0), bottomTexCoord));
		vertices.push_back(VertexStructure(offset + glm::vec3(-halfSize.x, -halfSize.y, -halfSize.z), glm::vec3(0, -1, 0), bottomTexCoord));
		vertices.push_back(VertexStructure(offset + glm::vec3(halfSize.x, -halfSize.y, -halfSize.z), glm::vec3(0, -1, 0), bottomTexCoord));
		vertices.push_back(VertexStructure(offset + glm::vec3(halfSize.x, -halfSize.y, halfSize.z), glm::vec3(0, -1, 0), bottomTexCoord));

		//Right
		vertices.push_back(VertexStructure(offset + glm::vec3(-halfSize.x, halfSize.y, -halfSize.z), glm::vec3(-1, 0, 0), rightTexCoord));
		vertices.push_back(VertexStructure(offset + glm::vec3(-halfSize.x, -halfSize.y, -halfSize.z), glm::vec3(-1, 0, 0), rightTexCoord));
		vertices.push_back(VertexStructure(offset + glm::vec3(-halfSize.x, -halfSize.y, halfSize.z), glm::vec3(-1, 0, 0), rightTexCoord));
		vertices.push_back(VertexStructure(offset + glm::vec3(-halfSize.x, halfSize.y, halfSize.z), glm::vec3(-1, 0, 0), rightTexCoord));

		//Left
		vertices.push_back(VertexStructure(offset + glm::vec3(halfSize.x, halfSize.y, halfSize.z), glm::vec3(1, 0, 0), leftTexCoord));
		vertices.push_back(VertexStructure(offset + glm::vec3(halfSize.x, -halfSize.y, halfSize.z), glm::vec3(1, 0, 0), leftTexCoord));
		vertices.push_back(VertexStructure(offset + glm::vec3(halfSize.x, -halfSize.y, -halfSize.z), glm::vec3(1, 0, 0), leftTexCoord));
		vertices.push_back(VertexStructure(offset + glm::vec3(halfSize.x, halfSize.y, -halfSize.z), glm::vec3(1, 0, 0), leftTexCoord));

		std::vector<unsigned int> indices;
		for (int i = 0; i < 6; i++)
		{
			indices.push_back(i * 4 + 0);
			indices.push_back(i * 4 + 1);
			indices.push_back(i * 4 + 3);
			indices.push_back(i * 4 + 1);
			indices.push_back(i * 4 + 2);
			indices.push_back(i * 4 + 3);
		}

		shader = ShaderLibrary::GetShader(shaderName);
		BindData(vertices, indices);
	}

	virtual void SetVertexAttributePointer() override
	{
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexStructure), (void*)0);
	}
};