#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

SpriteSheet::SpriteSheet()
{
	m_spriteSheetTexture = nullptr;
	m_spriteLayout = IntVector2(0, 0);
}

SpriteSheet::SpriteSheet(Texture* texture, int tilesWide, int tilesHigh)
{
	m_spriteSheetTexture = texture;
	m_spriteLayout = IntVector2(tilesWide, tilesHigh);
}

SpriteSheet::~SpriteSheet()
{

}

AABB2D SpriteSheet::GetTexCoordsForSpriteCoords(int spriteX, int spriteY) const
{
	Vector2 texCoordsMin;
	texCoordsMin.x = RangeMap1D((float) (spriteX), 0.f, (float) m_spriteLayout.x * 2.f, -1.f, 1.f);
	texCoordsMin.y = RangeMap1D((float) (spriteY), 0.f, (float) m_spriteLayout.y  * 2.f, -1.f, 1.f);
	Vector2 texCoordsMax;
	texCoordsMax.x = RangeMap1D((float) (spriteX + 1.f), 0.f, (float) m_spriteLayout.x * 2.f, -1.f, 1.f);
	texCoordsMax.y = RangeMap1D((float) (spriteY + 1.f), 0.f, (float) m_spriteLayout.y * 2.f, -1.f, 1.f);

	return AABB2D(texCoordsMin, texCoordsMax);
}

AABB2D SpriteSheet::GetTexCoordsForSpriteIndex(int spriteIndex) const
{
	int spritePosX = spriteIndex % m_spriteLayout.x;
	int spritePosY = (spriteIndex / m_spriteLayout.y);
	
	return GetTexCoordsForSpriteCoords(spritePosX, spritePosY);
}

int SpriteSheet::GetNumSprites() const
{
	return m_spriteLayout.CalcProduct();
}

Texture* SpriteSheet::GetSpriteSheetTexture() const
{
	return m_spriteSheetTexture;
}

void SpriteSheet::SetSpriteSheetTexture(Texture* texture)
{
	m_spriteSheetTexture = texture;
}
