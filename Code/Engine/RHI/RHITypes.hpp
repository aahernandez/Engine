#pragma once

enum e_RHIOutputMode : unsigned int
{
	RENDEROUTPUT_WINDOWED,
	RENDEROUTPUT_BORDERLESS,
	RENDEROUTPUT_FULLSCREEN_WINDOW,     
	RENDEROUTPUT_FULLSCREEN_DEDICATED,  
};

enum e_BufferUsage : unsigned int
{
	BUFFERUSAGE_GPU,     
	BUFFERUSAGE_STATIC,  
	BUFFERUSAGE_DYNAMIC, 
	BUFFERUSAGE_STAGING, 
};

//------------------------------------------------------------------------
enum e_PrimitiveType : unsigned int
{
	PRIMITIVE_NONE,
	PRIMITIVE_POINTS,
	PRIMITIVE_LINES,
	PRIMITIVE_TRIANGLES,
};

template <typename T>
inline void MemZero(T *ptr)
{
	memset(ptr, 0, sizeof(T));
}