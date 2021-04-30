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

VertexRenderObject::~VertexRenderObject()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void RenderObject::LoadTexture(const std::string& textureName, int textureNum)
{
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	int width, height, nrChannels;
	unsigned char* data = stbi_load(textureName.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		DPrint("Failed to load texture");
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);
}