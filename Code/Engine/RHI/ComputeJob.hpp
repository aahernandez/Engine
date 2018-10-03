#pragma once
#include "Engine/Math/IntVector3.hpp"

class ComputeShaderProgram;
class Texture2D;

class ComputeJob
{
public:
	ComputeShaderProgram *m_computeShader;
	Texture2D *m_uavTexture;
	unsigned int m_textureIndex;
	IntVector3 m_gridDimensions;

	ComputeJob();
	~ComputeJob();

	void SetShader(ComputeShaderProgram *shader);
	void SetGridDimensions(unsigned int width, unsigned int height, unsigned int depth = 1);
	void SetResourceUAV(unsigned int index, Texture2D *uavTexture);
};