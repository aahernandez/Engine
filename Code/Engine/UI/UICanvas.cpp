#pragma once
#include "Engine/UI/UICanvas.hpp"
#include "Engine/RHI/Texture2D.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"

UICanvas::UICanvas()
	: m_referenceResolution(720.f)
	, m_renderTarget(nullptr)
{
}

UICanvas::~UICanvas()
{
}

void UICanvas::SetReferenceResolution(float newResolution)
{
	m_referenceResolution = newResolution;
}

void UICanvas::SetRenderTarget(Texture2D *newTarget)
{
	m_renderTarget = newTarget;
	SetSize(Vector2::ZERO, Vector2((float)m_renderTarget->GetWidth(), (float)m_renderTarget->GetHeight()) );
}

void UICanvas::Render(SimpleRenderer *renderer)
{
	RenderChildElements(renderer);
}
