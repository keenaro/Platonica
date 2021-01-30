#include "RenderObject.h"
#include "Window.h"
#include <glad/glad.h>

RenderObject::RenderObject(bool addToRenderList /* = true */)
{
	if(addToRenderList)
	{
		Window::Instance().StartRenderingObject(this);
	}
}

RenderObject::~RenderObject()
{
	Window::Instance().StopRenderingObject(this);
}

void RenderObject::BindData(const std::vector<float>& vertices, const std::vector<unsigned int>& indices)
{
	IndiceCount = indices.size();

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size() , indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

void RenderObject::Draw()
{
	shader->Use();
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, IndiceCount, GL_UNSIGNED_INT, 0);
}