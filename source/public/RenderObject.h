#pragma once
#include "ShaderHelper.h"
#include <vector>

class RenderObject
{
public:
	RenderObject(bool addToRenderList = true);
	~RenderObject();

	virtual void Draw();
	virtual bool ShouldDraw() const { return true; };

	Shader* GetShader() const { return shader; };

protected:
	virtual void SetShaderUniformValues() {};

protected:
	Shader* shader;
};

class VertexRenderObject : public RenderObject
{
public:
	VertexRenderObject(bool addToRenderList = true) : RenderObject(addToRenderList) { };

protected:
	virtual void SetVertexAttributePointer();
	virtual void Draw() override;
	virtual bool ShouldDraw() const override;

private:
	unsigned int VAO, VBO, EBO, IndiceCount;

protected:
	template <typename T>
	void BindData(const std::vector<T>& vertices, const std::vector<unsigned int>& indices)
	{
		IndiceCount = indices.size();

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(T) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices.size(), indices.data(), GL_STATIC_DRAW);

		SetVertexAttributePointer();
		glEnableVertexAttribArray(0);
	}
};

