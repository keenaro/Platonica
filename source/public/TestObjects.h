#pragma once
#include "RenderObject.h"

class BoxObject : public VertexRenderObject
{
public:
	BoxObject::BoxObject(bool addToRenderList = true) : VertexRenderObject(addToRenderList)
	{
		const std::vector<float> vertices
		{
			 0.1f,  0.1f, 0.0f,
			 0.1f, -0.1f, 0.0f,
			-0.1f, -0.1f, 0.0f,
			-0.1f,  0.1f, 0.0f,
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