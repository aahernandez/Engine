#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Math/AABB2D.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

BitmapFont::BitmapFont()
{
	m_spriteSheet = nullptr;
}

BitmapFont::~BitmapFont()
{
	delete m_spriteSheet;
	m_spriteSheet = nullptr;
}

AABB2D BitmapFont::GetTexCoordsForGlyph(int glyphUnicode) const
{
	return m_spriteSheet->GetTexCoordsForSpriteIndex(glyphUnicode);
}
