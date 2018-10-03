#pragma once
#include "Engine/UI/UIPanel.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"

UIPanel::UIPanel()
	: m_tint(0.f, 0.f, 0.f, 0.f)
{

}

UIPanel::~UIPanel()
{

}

void UIPanel::SetTint(const RGBA &newTint)
{
	m_tint = newTint;
}

void UIPanel::Render(SimpleRenderer *renderer)
{
	AABB2D bounds = GetElementBounds();
	renderer->DrawQuad2D(bounds.mins, bounds.maxs, Vector2::ZERO, Vector2::ONE, m_tint);
	RenderChildElements(renderer);
}
