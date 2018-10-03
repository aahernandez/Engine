#pragma once
#include "Engine/UI/UISprite.hpp"
#include "Engine/RHI/Texture2D.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"

UISprite::UISprite()
	: m_texture(nullptr)
{
}

UISprite::~UISprite()
{
	delete m_texture;
}

void UISprite::SetSprite(Texture2D *newTexture)
{
	m_texture = newTexture;
}

void UISprite::SetSprite(RHIDevice *device, std::string imagePath)
{
	m_texture = new Texture2D(device, imagePath.c_str());
}

void UISprite::SetToNativeSize()
{
	SetSize(Vector2::ZERO, Vector2((float)m_texture->GetWidth(), (float)m_texture->GetHeight()) );
}

void UISprite::Render(SimpleRenderer *renderer)
{
	AABB2D bounds = GetLocalBounds();
	renderer->DrawQuad2DTextured(bounds, AABB2D::ZERO_TO_ONE, m_texture);
}
