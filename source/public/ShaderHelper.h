#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <string>
#include <functional>
#include "Defs.h"

class Shader;

class ShaderLibrary
{
public:
	static bool LoadShader(std::string& shaderName);
	static SharedPtr<Shader> GetShader(std::string& shaderName);
	static void SetShaderInUse(SharedPtr<Shader> shader) { currentlyBindedShader = shader; }
	static bool IsShaderCurrentlyBinded(SharedPtr<Shader> shader) { return currentlyBindedShader == shader; }

private:
	static std::map<std::size_t, SharedPtr<Shader>> shaderMap;
	static SharedPtr<Shader> currentlyBindedShader;
};


class Shader : public std::enable_shared_from_this<Shader>
{
public:
	unsigned int ID;

	void Use() { glUseProgram(ID); ShaderLibrary::SetShaderInUse(shared_from_this()); };
	void SetBool(const std::string& name, const bool value) const { glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); };
	void SetInt(const std::string& name, const int value) const { glUniform1i(glGetUniformLocation(ID, name.c_str()), value); };
	void SetFloat(const std::string& name, const float value) const { glUniform1f(glGetUniformLocation(ID, name.c_str()), value); };
	void SetVector3(const std::string& name, const glm::vec3& value) const { glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value)); };
	void SetIVector3(const std::string& name, const glm::ivec3& value) const { glUniform3iv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value)); };
	void SetVector4(const std::string& name, const glm::vec4& value) const { glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value)); };
	void SetMatrix4(const std::string& name, const glm::mat4& value) const { glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value)); };
};
