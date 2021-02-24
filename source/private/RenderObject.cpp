#include "RenderObject.h"
#include "Window.h"
#include <glad/glad.h>

RenderObject::RenderObject(bool addToRenderList)
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

void RenderObject::Draw()
{
	if (!ShaderLibrary::IsShaderCurrentlyBinded(shader))
	{
		shader->Use();
	}
	SetShaderUniformValues();
}

bool VertexRenderObject::ShouldDraw() const
{
	return RenderObject::ShouldDraw() && IndiceCount; 
}

void VertexRenderObject::Draw()
{
	RenderObject::Draw();
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, IndiceCount, GL_UNSIGNED_INT, 0);
}

void VertexRenderObject::SetVertexAttributePointer()
{
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}