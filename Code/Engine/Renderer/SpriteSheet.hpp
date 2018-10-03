#pragma once
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/AABB2D.hpp"
#include <string>

class SpriteSheet
{
public:
	SpriteSheet();
	SpriteSheet(Texture* texture, int tilesWide, int tilesHigh);
	~SpriteSheet();

	AABB2D GetTexCoordsForSpriteCoords(int spriteX, int spriteY) const;
	AABB2D GetTexCoordsForSpriteIndex(int spriteIndex) const;
	int GetNumSprites() const;
	Texture* GetSpriteSheetTexture() const;

	void SetSpriteSheetTexture(Texture* texture);

private:
	Texture* 	m_spriteSheetTexture;
	IntVector2	m_spriteLayout;
};
