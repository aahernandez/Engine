#pragma once
#include "Engine/UI/UIElement.hpp"

class Texture2D;

class UICanvas : public UIElement
{
public:
	float m_referenceResolution;
	Texture2D *m_renderTarget;

	UICanvas();
	~UICanvas();

	void SetReferenceResolution(float newResolution);
	void SetRenderTarget(Texture2D *newTarget);

	virtual void Render(SimpleRenderer *renderer) override;
};