#pragma once
#include "Engine/Math/IntVector2.hpp"
#include <string>

class Texture
{
	friend class Renderer;

private:
	std::string m_imageFilePath;
	unsigned int m_textureID;
	IntVector2 m_texelDimensions;

	Texture();
	~Texture();
};




