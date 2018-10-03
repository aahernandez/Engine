#pragma once
#include "Engine/Math/IntVector2.hpp"
#include <map> 

class Texture2D;
class RHIDevice;

struct t_FontData
{
	int x;
	int y;
	int width;
	int height;
	int xOffset;
	int yOffset;
	int xAdvance;
};

class Font
{
public:
	int m_exportSize;
	bool m_bold;
	bool m_italic;
	float m_lineHeight; //how much to move the cursor for the next line
	std::string m_fontName;
	IntVector2 m_scale;
	Texture2D *m_texture;
	std::string m_textureFilePath;
	std::map< int, t_FontData* > m_glyphMap;
	std::map< std::pair<int, int>, int > m_kearningMap;

	Font(char const *filename);
	~Font();

	void CreateFontFromFile(char const *filename);
	float CalculateTextWidth(char const *text, float scale = 1.f);
	float CalculateTextHeight(char const *text, float scale = 1.f);
	float GetMaxTextHeight(char const *text, float scale = 1.f);
	float GetMaxYOffset(char const *text, float scale = 1.f);
	float GetMinYOffset(char const *text, float scale = 1.f);
	t_FontData* GetGlyph(int character);
	t_FontData* GetInvalidGlyph();
	int GetKearning(int prevChar, int nextChar);
	void CreateTexture(RHIDevice *device);
	void Destroy();
};