#include "Engine/RHI/ComputeJob.hpp"

ComputeJob::ComputeJob()
	: m_computeShader(nullptr)
	, m_uavTexture(nullptr)
	, m_textureIndex(0)
{

}

ComputeJob::~ComputeJob()
{
	m_computeShader = nullptr;
	m_uavTexture = nullptr;
}

void ComputeJob::SetShader(ComputeShaderProgram *shader)
{
	m_computeShader = shader;
}

void ComputeJob::SetGridDimensions(unsigned int width, unsigned int height, unsigned int depth /*= 1*/)
{
	m_gridDimensions.x = width;
	m_gridDimensions.y = height;
	m_gridDimensions.z = depth;
}

void ComputeJob::SetResourceUAV(unsigned int index, Texture2D *uavTexture)
{
	m_textureIndex = index;
	m_uavTexture = uavTexture;
}
