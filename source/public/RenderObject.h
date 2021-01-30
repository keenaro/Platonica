#pragma once
#include "ShaderHelper.h"
#include <vector>

class RenderObject
{
public:
	RenderObject(bool addToRenderList = true);
	~RenderObject();

	Shader* shader;

	void BindData(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
	virtual void Draw();

private:
	unsigned int VAO, VBO, EBO, IndiceCount;
};

