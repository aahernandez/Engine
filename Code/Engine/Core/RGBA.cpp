#include "Engine/Core/RGBA.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Core/StringUtils.hpp"

RGBA::RGBA()
	: r(255)
	, g(255)
	, b(255)
	, a(255)
{
}

RGBA::RGBA(unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte)
	: r(redByte)
	, g(greenByte)
	, b(blueByte)
	, a(alphaByte)
{
}

RGBA::RGBA(float normalizedRed, float normalizedGreen, float normalizedBlue, float normalizedAlpha)
{
	r = (unsigned char) (normalizedRed * 255.f);
	g = (unsigned char) (normalizedGreen * 255.f);
	b = (unsigned char) (normalizedBlue * 255.f);
	a = (unsigned char) (normalizedAlpha * 255.f);
}

RGBA::~RGBA()
{

}

void RGBA::SetAsBytes(unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte = 255)
{
	r = redByte;
	g = greenByte;
	b = blueByte;
	a = alphaByte;
}

void RGBA::SetAsFloats(float normalizedRed, float normalizedGreen, float normalizedBlue, float normalizedAlpha = 1.0f)
{
	r = (unsigned char)(normalizedRed * 255);
	g = (unsigned char)(normalizedGreen * 255);
	b = (unsigned char)(normalizedBlue * 255);
	a = (unsigned char)(normalizedAlpha * 255);
}

void RGBA::GetAsFloats(float& out_normalizedRed, float& out_normalizedGreen, float& out_normalizedBlue, float& out_normalizedAlpha) const
{
	out_normalizedRed = (r / 255.f);
	out_normalizedGreen = (g / 255.f);
	out_normalizedBlue = (b / 255.f);
	out_normalizedAlpha = (a / 255.f);
}

void RGBA::ScaleRGB(float rgbScale)
{
	r *= (unsigned char)rgbScale;
	g *= (unsigned char)rgbScale;
	b *= (unsigned char)rgbScale;
;
	if (r > 255)
		r = 255;
	if (g > 255)
		g = 255;
	if (b > 255)
		b = 255;

	if (r < 0)
		r = 0;
	if (g < 0)
		g = 0;
	if (b < 0)
		b = 0;
}

void RGBA::ScaleAlpha(float alphaScale)
{
	a = (unsigned char) ((float) a * alphaScale);

	if (a > 255)
		a = 255;
	if (a < 0)
		a = 0;
}

void RGBA::AddAlpha(unsigned char additiveScale)
{
	a += additiveScale;

	if (a > 255)
		a = 255;
	if (a < 0)
		a = 0;
}

void RGBA::SubtractAlpha(unsigned char additiveScale)
{
	a -= additiveScale;

	if (a > 255)
		a = 255;
	if (a < 0)
		a = 0;
}

std::string RGBA::GetAsString()
{
	float rF, gF, bF, aF;
	GetAsFloats(rF, gF, bF, aF);
	std::string colorAsString = Stringf("{%f,%f,%f}", round(rF), round(gF), round(bF));
	return colorAsString;
}

RGBA RGBA::RangeMapBetweenColors(const RGBA& startColor, const RGBA& endColor, float percentage)
{
	RGBA newColor;
	newColor.r = (unsigned char) RangeMap1D(percentage, Vector2(0.f, 1.f), Vector2((float)startColor.r, (float)endColor.r) );
	newColor.g = (unsigned char) RangeMap1D(percentage, Vector2(0.f, 1.f), Vector2((float)startColor.g, (float)endColor.g) );
	newColor.b = (unsigned char) RangeMap1D(percentage, Vector2(0.f, 1.f), Vector2((float)startColor.b, (float)endColor.b) );
	return newColor;
}

RGBA RGBA::GetColorFromString(const std::string &colorName)
{
	if (colorName == "red")
		return RGBA::RED;
	if (colorName == "green")
		return RGBA::GREEN;
	if (colorName == "blue")
		return RGBA::BLUE;
	if (colorName == "black")
		return RGBA::BLACK;
	if (colorName == "orange")
		return RGBA::ORANGE;
	if (colorName == "yellow")
		return RGBA::YELLOW;
	if (colorName == "purple")
		return RGBA::PURPLE;
	if (colorName == "pink")
		return RGBA::PINK;
	if (colorName == "grey")
		return RGBA::GREY;
	if (colorName == "silver")
		return RGBA::SILVER;
	if (colorName == "teal")
		return RGBA::TEAL;
	if (colorName == "brown")
		return RGBA::BROWN;

	return RGBA::WHITE;
}

bool RGBA::operator==(const RGBA& color) const
{
	return r == color.r && g == color.g && b == color.b && a == color.a;
}

const RGBA RGBA::RED(255, 0, 0);
const RGBA RGBA::GREEN(0, 255, 0);
const RGBA RGBA::GREEN_LIGHT(77, 230, 77);
const RGBA RGBA::GREEN_DARK(0, 180, 0);
const RGBA RGBA::GREEN_ALPHA((unsigned char) 0, 255, 0, 0);
const RGBA RGBA::BLUE(0, 0, 255);
const RGBA RGBA::BLUE_LIGHT(0, 255, 255);
const RGBA RGBA::BLACK(0, 0, 0);
const RGBA RGBA::WHITE(255, 255, 255);
const RGBA RGBA::WHITE_TRANSPARENT((unsigned char)255, 255, 255, 175);
const RGBA RGBA::ORANGE(255, 153, 0);
const RGBA RGBA::YELLOW(255, 255, 0);
const RGBA RGBA::PURPLE(255, 0, 255);
const RGBA RGBA::PINK(255, 192, 203);
const RGBA RGBA::PINK_DEEP(255, 20, 147);
const RGBA RGBA::GREY(40, 40, 40);
const RGBA RGBA::GREY_LIGHT(70, 70, 70);
const RGBA RGBA::SILVER(192, 192, 192);
const RGBA RGBA::TEAL(33, 182, 168);
const RGBA RGBA::BROWN(153, 102, 51);
const RGBA RGBA::BROWN_LIGHT(150, 100, 25);
const RGBA RGBA::PURPLE_ALPHA((unsigned char) 255, 0, 255, 0);
const RGBA RGBA::ALPHA_FULL((unsigned char) 0, 0, 0, 0);
const RGBA RGBA::OVERLAY_DARK((unsigned char) 0, 0, 0, 200);
const RGBA RGBA::BLACK_OVERLAY_DARK((unsigned char) 0, 0, 0, 230);