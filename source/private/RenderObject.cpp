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

void RenderObject::SetVertexAttributePointer()
{
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

void RenderObject::Draw()
{
	shader->Use();
	SetShaderUniformValues();
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, IndiceCount, GL_UNSIGNED_INT, 0);
}