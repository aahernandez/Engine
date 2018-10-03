#pragma once
#include "Engine/Math/AABB2D.hpp"
#include "Engine/Core/RGBA.hpp"
#include <string>
#include <functional>

class UIButton
{
public:
	UIButton();
	UIButton(const AABB2D& newBoundingBox, const std::string& newText, const RGBA& buttonColor = RGBA::RED, const RGBA& textColor = RGBA::WHITE);
	~UIButton();

	void RunFunction();

	void SetBoundingBox(const AABB2D& newBoundingBox);
	void SetText(const std::string& newText);
	void SetButtonColor(const RGBA& newButtonColor);
	void SetTextColor(const RGBA& newTextColor);
	void SetFunction(const std::function<void()>& newFunction);

	AABB2D GetBoundingBox() const;
	std::string GetText() const;
	RGBA GetBoundingBoxColor() const;
	RGBA GetTextColor() const;

private:
	AABB2D m_boundingBox;
	std::string m_text;
	RGBA m_buttonColor;
	RGBA m_textColor;
	std::function<void()> m_function;
};