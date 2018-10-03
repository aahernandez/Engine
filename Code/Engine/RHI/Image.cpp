#include "Engine/RHI/Image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_PSD
#define STBI_NO_PNM
#include "ThirdParty/STB/stb_image.h"

Image::Image() 
	: m_buffer(nullptr)
	, m_width(0)
	, m_height(0)
	, m_bpp(0)
{
}

Image::~Image()
{
	Destroy();
}

bool Image::LoadFromFile(char const *filename)
{
	Destroy();
	
	int w;
	int h;
	int comp;

	unsigned char *data = stbi_load(filename, &w, &h, &comp, 0);
	if (data != nullptr) {
		if (comp == 3) {
			unsigned char *new_buffer = stbi__convert_format(data, comp, 4, w, h);
			data = new_buffer;
		}

		m_buffer = (unsigned char *)data;
		m_width = w;
		m_height = h;
		m_bpp = 4;
		m_format = IMAGEFORMAT_RGBA8;
		return true;
	}

	return false;
}

bool Image::CreateClear(unsigned int width, unsigned int height, RGBA color)
{
	Destroy();

	m_width = width;
	m_height = height;
	m_format = IMAGEFORMAT_RGBA8;
	m_bpp = 4;

	unsigned int size = m_width * m_height * m_bpp;
	m_buffer = (unsigned char*) ::malloc(size);
	RGBA col = color;

	RGBA *colors = (RGBA*)m_buffer;
	unsigned int count = m_width * m_height;

	for (unsigned int counter = 0; counter < count; ++counter)
	{
		colors[counter] = col;
	}

	return true;
}

void Image::Destroy()
{
	if (nullptr != m_buffer) {
		stbi_image_free(m_buffer);
	}
}
