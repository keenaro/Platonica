#include "RenderObject.h"

class BoxObject : public RenderObject
{
public:
	BoxObject::BoxObject(bool addToRenderList = true) : RenderObject(addToRenderList)
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

class TriangleObject : public RenderObject
{
public:
	TriangleObject::TriangleObject(bool addToRenderList = true) : RenderObject(addToRenderList)
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