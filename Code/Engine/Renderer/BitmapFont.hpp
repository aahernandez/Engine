#pragma once
#include <string>

class AABB2D;
class SpriteSheet;

class BitmapFont
{
	friend class Renderer;

public:
	AABB2D GetTexCoordsForGlyph(int glyphUnicode) const;
	float GetGlyphAspect(int glyphUnicode) const { glyphUnicode;  1.0f; }

private:
	BitmapFont();
	~BitmapFont();
	SpriteSheet* m_spriteSheet;
};