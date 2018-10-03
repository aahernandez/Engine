#include "Engine/UI/UIButton.hpp"

UIButton::UIButton()
	: m_boundingBox(AABB2D::ZERO_TO_ONE)
	, m_text("")
	, m_buttonColor(RGBA::RED)
	, m_textColor(RGBA::WHITE)
{

}

UIButton::UIButton(const AABB2D& newBoundingBox, const std::string& newText, const RGBA& buttonColor /*= RGBA::GREY*/, const RGBA& textColor /*= RGBA::WHITE*/)
	: m_boundingBox(newBoundingBox)
	, m_text(newText)
	, m_buttonColor(buttonColor)
	, m_textColor(textColor)
{

}

UIButton::~UIButton()
{

}

void UIButton::SetBoundingBox(const AABB2D& newBoundingBox)
{
	m_boundingBox = newBoundingBox;
}

void UIButton::SetText(const std::string& newText)
{
	m_text = newText;
}

void UIButton::SetButtonColor(const RGBA& newButtonColor)
{
	m_buttonColor = newButtonColor;
}

void UIButton::SetTextColor(const RGBA& newTextColor)
{
	m_textColor = newTextColor;
}

void UIButton::SetFunction(const std::function<void()>& newFunction)
{
	m_function = newFunction;
}

AABB2D UIButton::GetBoundingBox() const
{
	return m_boundingBox;
}

std::string UIButton::GetText() const
{
	return m_text;
}

RGBA UIButton::GetBoundingBoxColor() const
{
	return m_buttonColor;
}

RGBA UIButton::GetTextColor() const
{
	return m_textColor;
}

void UIButton::RunFunction()
{
	m_function();
}

