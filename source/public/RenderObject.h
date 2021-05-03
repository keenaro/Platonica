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

	SharedPtr<Shader> GetShader() const { return shader; };

protected:
	virtual void SetShaderUniformValues() {};
	void LoadTexture(const std::string& textureName, int textureNum = 0);

public:
	SharedPtr<Shader> shader;
	GLuint texture = 0;
};

class VertexRenderObject : public RenderObject
{
public:
	VertexRenderObject(bool addToRenderList = true) : RenderObject(addToRenderList) { };
	~VertexRenderObject();

public:
	virtual void Draw() override;

protected:
	virtual void SetVertexAttributePointer();
	virtual bool ShouldDraw() const override;

protected:
	unsigned int VAO = 0, VBO = 0, EBO = 0, IndiceCount = 0;

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
		glBufferData(GL_ARRAY_BUFFER, sizeof(T) * vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices.size(), indices.data(), GL_DYNAMIC_DRAW);

		SetVertexAttributePointer();
		glEnableVertexAttribArray(0);

	}
};

