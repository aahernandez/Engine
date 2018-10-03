#pragma once
#include <string>

class RGBA
{
public:
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;

	static const RGBA RED;
	static const RGBA GREEN;
	static const RGBA GREEN_LIGHT;
	static const RGBA GREEN_DARK;
	static const RGBA GREEN_ALPHA;
	static const RGBA BLUE;
	static const RGBA BLUE_LIGHT;
	static const RGBA BLACK;
	static const RGBA WHITE;
	static const RGBA WHITE_TRANSPARENT;
	static const RGBA ORANGE;
	static const RGBA YELLOW;
	static const RGBA PURPLE;
	static const RGBA PINK;
	static const RGBA PINK_DEEP;
	static const RGBA GREY;
	static const RGBA GREY_LIGHT;
	static const RGBA SILVER;
	static const RGBA BROWN;
	static const RGBA TEAL;
	static const RGBA BROWN_LIGHT;
	static const RGBA PURPLE_ALPHA;
	static const RGBA ALPHA_FULL;
	static const RGBA OVERLAY_DARK;
	static const RGBA BLACK_OVERLAY_DARK;

	RGBA();
	RGBA(unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte = 255);
	explicit RGBA(float normalizedRed, float normalizedGreen, float normalizedBlue, float normalizedAlpha);
	~RGBA();

	void SetAsBytes(unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte);
	void SetAsFloats(float normalizedRed, float normalizedGreen, float normalizedBlue, float normalizedAlpha);
	void GetAsFloats(float& out_normalizedRed, float& out_normalizedGreen, float& out_normalizedBlue, float& out_normalizedAlpha) const;
	void ScaleRGB(float rgbScale);
	void ScaleAlpha(float alphaScale);
	void AddAlpha(unsigned char additiveScale);
	void SubtractAlpha(unsigned char additiveScale);
	std::string GetAsString();

	static RGBA RangeMapBetweenColors(const RGBA& startColor, const RGBA& endColor, float percentage);
	static RGBA GetColorFromString(const std::string &colorName);

	bool operator==(const RGBA& color) const;
};