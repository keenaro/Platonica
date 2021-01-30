#include "ShaderHelper.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "Defs.h"

std::map<std::size_t, Shader> ShaderLibrary::shaderMap;

bool ShaderLibrary::LoadShader(std::string& shaderName)
{
	DPRINTF("Shader: Attempting to load %s.\n", shaderName.c_str());

	std::string vertSource;
	std::string fragSource;
	std::ifstream vertShaderFile;
	std::ifstream fragShaderFile;

	vertShaderFile.open("shaders/" + shaderName + ".vert", std::ifstream::in);
	if(vertShaderFile.is_open())
	{
		std::stringstream vertShaderStream;
		vertShaderStream << vertShaderFile.rdbuf();
		vertShaderFile.close();
		vertSource = vertShaderStream.str();
	}
	else 
	{
		DPRINTF("Shader: %s.vert does not exist.\n", shaderName.c_str());
		return false;
	}

	fragShaderFile.open("shaders/" + shaderName + ".frag", std::ifstream::in);
	if (fragShaderFile.is_open())
	{
		std::stringstream fragShaderStream;
		fragShaderStream << fragShaderFile.rdbuf();
		fragShaderFile.close();
		fragSource = fragShaderStream.str();
	}
	else
	{
		DPRINTF("Shader: %s.frag does not exist.\n", shaderName.c_str());
		return false;
	}

	unsigned int vertex, fragment;
	Shader shader;
	int shaderStatus;
	char shaderInfoLog[512];

	vertex = glCreateShader(GL_VERTEX_SHADER);
	{
		const char* vertSourceData = vertSource.c_str();
		glShaderSource(vertex, 1, &vertSourceData, NULL);
		glCompileShader(vertex);
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &shaderStatus);
		if (!shaderStatus)
		{
			glGetShaderInfoLog(vertex, 512, NULL, shaderInfoLog);
			DPRINTF("Shader: %s failed vertex compilation\n%s\n", shaderInfoLog);
			return false;
		};
	}

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	{
		const char* fragSourceData = fragSource.c_str();
		glShaderSource(fragment, 1, &fragSourceData, NULL);
		glCompileShader(fragment);
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &shaderStatus);
		if (!shaderStatus)
		{
			glGetShaderInfoLog(fragment, 512, NULL, shaderInfoLog);
			DPRINTF("Shader: %s failed fragment compilation\n%s", shaderInfoLog);
			return false;
		};
	}

	shader.ID = glCreateProgram();
	{
		glAttachShader(shader.ID, vertex);
		glAttachShader(shader.ID, fragment);
		glLinkProgram(shader.ID);

		glGetProgramiv(shader.ID, GL_LINK_STATUS, &shaderStatus);
		if (!shaderStatus)
		{
			glGetProgramInfoLog(shader.ID, 512, NULL, shaderInfoLog);
			DPRINTF("Shader: %s failed linking\n%s", shaderInfoLog);
			return false;
		}

		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	shaderMap[std::hash<std::string>{} (shaderName)] = shader;
	DPRINTF("Shader: Loaded %s.\n", shaderName.c_str());

	return true;
}

Shader* ShaderLibrary::GetShader(std::string& shaderName)
{
	const std::size_t hash = std::hash<std::string>{} (shaderName);
	std::map<std::size_t, Shader>::iterator search = shaderMap.find(hash);
	if(search == shaderMap.end())
	{
		if(LoadShader(shaderName))
		{
			search = shaderMap.find(hash);
		}
		else
		{
			DPRINTF("Shader: Failed to get %s.\n", shaderName.c_str());
			return nullptr;
		}
	}

	return &search->second;
}