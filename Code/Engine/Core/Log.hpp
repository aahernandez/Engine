#pragma once
#include "Engine/Core/RGBA.hpp"
#include <string>
#include <vector>

class Vector2;

struct ColoredText
{
public:
	std::string m_text;
	RGBA m_color;
	float m_scale;
};

enum ScaleSizing
{
	TITLE,
	DEFAULT,
	SUBTITLE,
	HEADING,
	PARAGRAPH,
	NUM_SIZING_SCALES
};

class Log
{
public:
	float m_windowScale;
	bool m_hasBackgroundBox;
	std::vector<ColoredText*> m_coloredLog;
	float m_scales[NUM_SIZING_SCALES];

	Log();
	Log(float scale, bool hasBackgroundBox);
	~Log();

	void StoreTextInLog(std::string text, RGBA color = RGBA::WHITE, ScaleSizing textScale = DEFAULT, bool isPushedFront = false);
	void StoreTextInLog(char const *text, RGBA color = RGBA::WHITE, ScaleSizing textScale = DEFAULT, bool isPushedFront = false);
	void StoreTextInLog(bool boolPassed, RGBA color = RGBA::WHITE, ScaleSizing textScale = DEFAULT, bool isPushedFront = false);
	void StoreTextInLog(char textChar, RGBA color = RGBA::WHITE, ScaleSizing textScale = DEFAULT, bool isPushedFront = false);

	void RenderLog(Vector2 position, bool isRenderingUp = false, bool isRenderingCenter = false);
	void EmptyLog();
	RGBA GetLogColorAtIndex(int index);
	void SetColorAtIndex(int index, RGBA newColor);
	void EraseLogAtIndex(int index);

	float GetMaxWidthOfLog();
	float GetHeightOfLog();

	void Destroy();
};