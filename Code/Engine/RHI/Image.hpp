#pragma once
#include "Engine/Core/RGBA.hpp"

#if !defined( __CORE_IMAGE__ )
#define __GAME_GAME__

enum ImageFormat_e : unsigned int
{
	IMAGEFORMAT_RGBA8,
	IMAGEFORMAT_RGBA8_UAV,
	IMAGEFORMAT_D24S8,
	NUM_IMAGEFORMATS,
};

class Image
{
public:
	unsigned char *m_buffer;
	unsigned int m_width;
	unsigned int m_height;
	unsigned int m_bpp;
	ImageFormat_e m_format;

	Image();
	~Image();

	bool LoadFromFile(char const *filename);
	bool CreateClear(unsigned int width, unsigned int height, RGBA color);
	void Destroy();

	inline bool IsValid() const { return nullptr != m_buffer; }
	inline unsigned int GetPixelSize() const { return m_bpp; }
	inline unsigned int GetWidth() const { return m_width; }
	inline unsigned int GetHeight() const { return m_height; }
	inline unsigned int GetStride() const { return m_width * m_bpp; }
	inline unsigned int GetSlice() const { return GetStride() * m_height; }
};

#endif 