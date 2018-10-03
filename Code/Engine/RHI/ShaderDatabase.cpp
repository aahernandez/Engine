#include "Engine/RHI/ShaderDatabase.hpp"
#include "Engine/RHI/ShaderProgram.hpp"
#include "Engine/RHI/RHIDevice.hpp"
#include "Engine/RHI/ComputeShaderProgram.hpp"
#include "Engine/Core/FileUtils.hpp"
#include <string>

ShaderDatabase::ShaderDatabase()
{
	m_shaderIter = m_shaders.begin();
	m_computeShaderIter = m_computeShaders.begin();
}

ShaderDatabase::~ShaderDatabase()
{
	Destroy();
}

void ShaderDatabase::CreateShaderFromFile(RHIDevice *device, char const *filename)
{
	std::string filePath = "Data/HLSL/";
	filePath += filename;
	filePath += ".hlsl";
	ShaderProgram *shader = device->CreateShaderFromHLSLFile(filePath.c_str());
	m_shaders[filename] = shader;
}

void ShaderDatabase::CreateComputeShaderFromFile(RHIDevice *device, char const *filepath)
{
	std::string filename = GetFileNameWithoutPathAndFileType(filepath);
	ComputeShaderProgram *computeShader = device->CreateComputeShaderFromHLSLFile(filepath);
	m_computeShaders[filename] = computeShader;
}

ComputeShaderProgram* ShaderDatabase::GetNextComputeShaderInMap()
{
	if (m_computeShaderIter == m_computeShaders.end())
	{
		m_computeShaderIter = m_computeShaders.begin();
	}
	m_computeShaderIter++;
	if (m_computeShaderIter == m_computeShaders.end())
	{
		m_computeShaderIter = m_computeShaders.begin();
	}

	return m_computeShaderIter->second;
}

ComputeShaderProgram* ShaderDatabase::GetPreviousComputeShaderInMap()
{
	if (m_computeShaderIter == m_computeShaders.begin())
	{
		m_computeShaderIter = m_computeShaders.end();
	}
	m_computeShaderIter--;

	return m_computeShaderIter->second;
}

ComputeShaderProgram* ShaderDatabase::GetCurrentComputeShaderInMap()
{
	if (m_computeShaderIter == m_computeShaders.end())
	{
		m_computeShaderIter = m_computeShaders.begin();
	}
	return m_computeShaderIter->second;
}

ComputeShaderProgram* ShaderDatabase::GetComputeShader(char const *shaderFileName)
{
	return m_computeShaders[shaderFileName];
}

ShaderProgram* ShaderDatabase::GetNextShaderInMap()
{
	if (m_shaderIter == m_shaders.end())
	{
		m_shaderIter = m_shaders.begin();
	}
	m_shaderIter++;
	if (m_shaderIter == m_shaders.end())
	{
		m_shaderIter = m_shaders.begin();
	}

	return m_shaderIter->second;
}

ShaderProgram* ShaderDatabase::GetPreviousShaderInMap()
{
	if (m_shaderIter == m_shaders.begin())
	{
		m_shaderIter = m_shaders.end();
	}
	m_shaderIter--;

	return m_shaderIter->second;
}

ShaderProgram* ShaderDatabase::GetCurrentShaderInMap()
{
	if (m_shaderIter == m_shaders.end())
	{
		m_shaderIter = m_shaders.begin();
	}
	return m_shaderIter->second;
}

ShaderProgram* ShaderDatabase::GetShader(char const *shaderFileName)
{
	return m_shaders[shaderFileName];
}

ShaderProgram* ShaderDatabase::GetUntexturedShaderInMap()
{
	if (m_shaderIter == m_shaders.end())
	{
		m_shaderIter = m_shaders.begin();
	}
	while (m_shaderIter->first != "color")
	{
		m_shaderIter++;
		if (m_shaderIter == m_shaders.end())
		{
			m_shaderIter = m_shaders.begin();
		}
	}
	return m_shaderIter->second;
}

void ShaderDatabase::Destroy()
{
	m_shaderIter = m_shaders.begin();
	for (m_shaderIter = m_shaders.begin(); m_shaderIter != m_shaders.end(); m_shaderIter = m_shaders.begin())
	{
		delete m_shaderIter->second;
		m_shaderIter->second = nullptr;
		m_shaders.erase(m_shaderIter);
	}

	m_computeShaderIter = m_computeShaders.begin();
	for (m_computeShaderIter = m_computeShaders.begin(); m_computeShaderIter != m_computeShaders.end(); m_computeShaderIter = m_computeShaders.begin())
	{
		delete m_computeShaderIter->second;
		m_computeShaderIter->second = nullptr;
		m_computeShaders.erase(m_computeShaderIter);
	}
}
