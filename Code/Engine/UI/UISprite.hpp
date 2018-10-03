#pragma once
#include "Engine/UI/UIElement.hpp"

#include <string>

class Texture2D;
class RHIDevice;

class UISprite : public UIElement
{
public:
	Texture2D *m_texture;

	UISprite();
	~UISprite();

	void SetSprite(Texture2D *newTexture);
	void SetSprite(RHIDevice *device, std::string imagePath);
	void SetToNativeSize();

	virtual void Render(SimpleRenderer *renderer) override;
};