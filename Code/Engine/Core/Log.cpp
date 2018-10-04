#include "Engine/Core/Log.hpp"
#include "Engine/RHI/SimpleRenderer.hpp"
#include "Engine/RHI/Font.hpp"

Log::Log()
	: m_hasBackgroundBox(false)
	, m_windowScale(1.f)
{
	m_scales[TITLE] = 1.2f;
	m_scales[DEFAULT] = 1.f;
	m_scales[SUBTITLE] = 0.8f;
	m_scales[HEADING] = 0.7f;
	m_scales[PARAGRAPH] = 0.5f;
}

Log::Log(float scale, bool hasBackgroundBox)
	: m_hasBackgroundBox(hasBackgroundBox)
	, m_windowScale(scale)
{
	m_scales[TITLE] = 1.2f;
	m_scales[DEFAULT] = 1.f;
	m_scales[SUBTITLE] = 0.8f;
	m_scales[HEADING] = 0.7f;
	m_scales[PARAGRAPH] = 0.5f;
}

Log::~Log()
{
	Destroy();
}

void Log::StoreTextInLog(std::string text, RGBA color /*= RGBA::WHITE*/, ScaleSizing textScale /*= DEFAULT*/, bool isPushedFront /*= false*/)
{
	ColoredText *newLog = new ColoredText();
	newLog->m_text = text.c_str();
	newLog->m_color = color;
	newLog->m_scale = m_scales[textScale];

	if (isPushedFront)
		m_coloredLog.insert(m_coloredLog.begin() + 1, newLog);
	else
		m_coloredLog.push_back(newLog);
}

void Log::StoreTextInLog(char const *text, RGBA color /*= RGBA::WHITE*/, ScaleSizing textScale /*= DEFAULT*/, bool isPushedFront /*= false*/)
{
	ColoredText *newLog = new ColoredText();
	newLog->m_text = text;
	newLog->m_color = color;
	newLog->m_scale = m_scales[textScale];

	if (isPushedFront)
		m_coloredLog.insert(m_coloredLog.begin() + 1, newLog);
	else
		m_coloredLog.push_back(newLog);
}

void Log::StoreTextInLog(bool boolPassed, RGBA color /*= RGBA::WHITE*/, ScaleSizing textScale /*= DEFAULT*/, bool isPushedFront /*= false*/)
{
	ColoredText *newLog = new ColoredText();
	if (boolPassed)
	{
		newLog->m_text = "true";
	}
	else
	{
		newLog->m_text = "false";
	}
	newLog->m_color = color;
	newLog->m_scale = m_scales[textScale];

	if (isPushedFront)
		m_coloredLog.insert(m_coloredLog.begin() + 1, newLog);
	else
		m_coloredLog.push_back(newLog);
}

void Log::StoreTextInLog(char textChar, RGBA color /*= RGBA::WHITE*/, ScaleSizing textScale /*= DEFAULT*/, bool isPushedFront /*= false*/)
{
	std::string tempString = "";
	tempString += textChar;
	ColoredText *newLog = new ColoredText();
	newLog->m_text = tempString.c_str();
	newLog->m_color = color;
	newLog->m_scale = m_scales[textScale];

	if (isPushedFront)
		m_coloredLog.insert(m_coloredLog.begin() + 1, newLog);
	else
		m_coloredLog.push_back(newLog);
}

void Log::RenderLog(Vector2 position, bool isRenderingUp /*= false*/, bool isRenderingCenter /*= false*/)
{
	if (m_hasBackgroundBox)
	{
		float logWidth = GetMaxWidthOfLog();
		float logHeight = GetHeightOfLog();
		float borderSize = 1.f;
		float borderSizeHalved = borderSize * 0.5f;
		Vector2 positionScaled = position * m_windowScale;
		AABB2D logBounds = AABB2D(Vector2(positionScaled.x - borderSize, positionScaled.y - logHeight - borderSize), Vector2(positionScaled.x + logWidth + borderSize, positionScaled.y));
		g_theSimpleRenderer->DrawDebugQuad2D(logBounds, borderSizeHalved, RGBA(0.f, 0.f, 0.f, 0.5f), RGBA::ORANGE);
	}

	float logYPos = 0.f;
	std::vector<ColoredText*>::iterator iter = m_coloredLog.begin();
	for (iter; iter != m_coloredLog.end(); ++iter)
	{
		if (isRenderingCenter)
			g_theSimpleRenderer->DrawText2DCenteredOnPosition((*iter)->m_text.c_str(), Vector2(position.x, position.y + logYPos), (*iter)->m_color, (*iter)->m_scale);
		else
			g_theSimpleRenderer->DrawText2D((*iter)->m_text.c_str(), Vector2(position.x, position.y + logYPos), (*iter)->m_color, (*iter)->m_scale, g_theSimpleRenderer->m_font);
		
		float totalScale = m_windowScale * (*iter)->m_scale;
		float newHeight = g_theSimpleRenderer->m_font->GetMaxTextHeight((*iter)->m_text.c_str(), totalScale);
		float newMaxYOffset = g_theSimpleRenderer->m_font->GetMaxYOffset((*iter)->m_text.c_str(), totalScale);
		float currentLineHeight = newHeight + newMaxYOffset;
		currentLineHeight;

		if (isRenderingUp)
			logYPos += 30.f; // (currentLineHeight / m_windowScale);
		else
			logYPos -= 30.f; //(currentLineHeight / m_windowScale);
	}
}

void Log::EmptyLog()
{
	std::vector<ColoredText*>::iterator textIter;
	for (textIter = m_coloredLog.begin(); textIter != m_coloredLog.end(); ++textIter)
	{
		delete *textIter;
		*textIter = nullptr;
	}
	m_coloredLog.clear();
}

RGBA Log::GetLogColorAtIndex(int index)
{
	if (m_coloredLog.empty())
		return RGBA::ALPHA_FULL;
	return m_coloredLog[index]->m_color;
}

void Log::SetColorAtIndex(int index, RGBA newColor)
{
	if (m_coloredLog.empty())
		return;
	m_coloredLog[index]->m_color = newColor;
}

void Log::EraseLogAtIndex(int index)
{
	if (m_coloredLog.empty())
		return;

	delete m_coloredLog[index];
	m_coloredLog[index] = nullptr;
	m_coloredLog.erase(m_coloredLog.begin() + index);
}

float Log::GetMaxWidthOfLog()
{
	float maxWidth = 0.f;

	std::vector<ColoredText*>::iterator iter = m_coloredLog.begin();
	for (iter; iter != m_coloredLog.end(); ++iter)
	{
		float totalScale = m_windowScale * (*iter)->m_scale;
		float textWidth = g_theSimpleRenderer->m_font->CalculateTextWidth((*iter)->m_text.c_str(), totalScale);
		if (textWidth > maxWidth)
			maxWidth = textWidth;
	}
	return maxWidth;
}

float Log::GetHeightOfLog()
{
	float height = 0.f;
	
	std::vector<ColoredText*>::iterator iter = m_coloredLog.begin();
	for (iter; iter != m_coloredLog.end(); ++iter)
	{
		float totalScale = m_windowScale * (*iter)->m_scale;

		float newHeight = g_theSimpleRenderer->m_font->GetMaxTextHeight((*iter)->m_text.c_str(), totalScale);
		float newMaxYOffset = g_theSimpleRenderer->m_font->GetMaxYOffset((*iter)->m_text.c_str(), totalScale);
		float currentLineHeight = newHeight + newMaxYOffset;

		height += currentLineHeight;
	}

	return height;
}

void Log::Destroy()
{
	EmptyLog();

	std::vector<ColoredText*>::iterator logIter = m_coloredLog.begin();
	for (logIter; logIter != m_coloredLog.end(); ++logIter)
	{
		logIter = m_coloredLog.erase(logIter);
		if (logIter == m_coloredLog.end())
			break;
	}
}
