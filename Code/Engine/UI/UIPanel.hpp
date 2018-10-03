#pragma once
#include "Engine/UI/UIElement.hpp"
#include "Engine/Core/RGBA.hpp"

class UIPanel : public UIElement
{
public:
	RGBA m_tint;

	UIPanel();
	~UIPanel();

	void SetTint(const RGBA &newTint);

	virtual void Render(SimpleRenderer *renderer) override;
};