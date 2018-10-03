#pragma once
#include <map>

class ShaderProgram;
class ComputeShaderProgram;
class RHIDevice;

class ShaderDatabase
{
public:
	std::map<std::string, ShaderProgram*> m_shaders;
	std::map<std::string, ShaderProgram*>::iterator m_shaderIter;
	std::map<std::string, ComputeShaderProgram*> m_computeShaders;
	std::map<std::string, ComputeShaderProgram*>::iterator m_computeShaderIter;

	ShaderDatabase();
	~ShaderDatabase();

	void CreateShaderFromFile(RHIDevice *device, char const *filename);
	ShaderProgram* GetNextShaderInMap();
	ShaderProgram* GetPreviousShaderInMap();
	ShaderProgram* GetCurrentShaderInMap();
	ShaderProgram* GetShader(char const *shaderFileName);
	ShaderProgram* GetUntexturedShaderInMap();

	void CreateComputeShaderFromFile(RHIDevice *device, char const *filepath);
	ComputeShaderProgram* GetNextComputeShaderInMap();
	ComputeShaderProgram* GetPreviousComputeShaderInMap();
	ComputeShaderProgram* GetCurrentComputeShaderInMap();
	ComputeShaderProgram* GetComputeShader(char const *shaderFileName);

	void Destroy();
};