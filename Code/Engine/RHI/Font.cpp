#include "Engine/RHI/Font.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/RHI/Texture2D.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <sstream>
#include <regex>

Font::Font(char const *filename)
	: m_texture(nullptr)
{
	CreateFontFromFile(filename);
}

Font::~Font()
{
	Destroy();
}

void Font::CreateFontFromFile(char const *filename)
{
	std::string filePath = "Data/Fonts/";
	filePath += filename;
	filePath += ".fnt";

	size_t fontFileSize = 0U;
	void *fontFileData = FileReadToBuffer(filePath.c_str(), &fontFileSize);

	std::stringstream stream;
	std::string value;
	stream.write((const char*)fontFileData, fontFileSize);

	std::string fontFileLine;
	std::smatch match;

	//info line
	std::getline(stream, fontFileLine);

	m_fontName = filename;
	m_fontName = m_fontName.substr(0, strlen(filename) - 2);

	m_textureFilePath = "Data/Fonts/";
	m_textureFilePath += filename;
	m_textureFilePath += "_0.png";

	std::regex expression("size=-?\\d+");
	std::regex_search(fontFileLine, match, expression);
	value = match[0];
	size_t stringLength = strlen("size=");
	value = value.substr(stringLength);
	m_exportSize = stoi(value);

	expression = "bold=-?\\d+";
	std::regex_search(fontFileLine, match, expression);
	value = match[0];
	stringLength = strlen("bold=");
	value = value.substr(stringLength);
	int tempInt = stoi(value);
	if (tempInt == 0)
		m_bold = false;
	else
		m_bold = true;

	expression = "italic=-?\\d+";
	std::regex_search(fontFileLine, match, expression);
	value = match[0];
	stringLength = strlen("italic=");
	value = value.substr(stringLength);
	tempInt = stoi(value);
	if (tempInt == 0)
		m_italic = false;
	else
		m_italic = true;

	//common line
	std::getline(stream, fontFileLine);

	expression = "lineHeight=-?\\d+";
	std::regex_search(fontFileLine, match, expression);
	value = match[0];
	stringLength = strlen("lineHeight=");
	value = value.substr(stringLength);
	m_lineHeight = stof(value);

	expression = "scaleW=-?\\d+";
	std::regex_search(fontFileLine, match, expression);
	value = match[0];
	stringLength = strlen("scaleW=");
	value = value.substr(stringLength);
	m_scale.x = stoi(value);

	expression = "scaleH=-?\\d+";
	std::regex_search(fontFileLine, match, expression);
	value = match[0];
	stringLength = strlen("scaleH=");
	value = value.substr(stringLength);
	m_scale.y = stoi(value);

	//page line
	std::getline(stream, fontFileLine);

	//chars line
	std::getline(stream, fontFileLine);
	int charsNum;
	expression = "chars count=-?\\d+";
	std::regex_search(fontFileLine, match, expression);
	value = match[0];
	stringLength = strlen("chars count=");
	value = value.substr(stringLength);
	charsNum = stoi(value);

	//char lines
	for (int charsCounter = 0; charsCounter < charsNum; charsCounter++)
	{
		std::getline(stream, fontFileLine);
		t_FontData* newFontData = new t_FontData();

		expression = "id=-?\\d+";
		std::regex_search(fontFileLine, match, expression);
		value = match[0];
		stringLength = strlen("id=");
		value = value.substr(stringLength);
		int charID = stoi(value);

		expression = "x=-?\\d+";
		std::regex_search(fontFileLine, match, expression);
		value = match[0];
		stringLength = strlen("x=");
		value = value.substr(stringLength);
		newFontData->x = stoi(value);

		expression = "y=-?\\d+";
		std::regex_search(fontFileLine, match, expression);
		value = match[0];
		stringLength = strlen("y=");
		value = value.substr(stringLength);
		newFontData->y = stoi(value);

		expression = "width=-?\\d+";
		std::regex_search(fontFileLine, match, expression);
		value = match[0];
		stringLength = strlen("width=");
		value = value.substr(stringLength);
		newFontData->width = stoi(value);

		expression = "height=-?\\d+";
		std::regex_search(fontFileLine, match, expression);
		value = match[0];
		stringLength = strlen("height=");
		value = value.substr(stringLength);
		newFontData->height = stoi(value);

		expression = "xoffset=-?\\d+";
		std::regex_search(fontFileLine, match, expression);
		value = match[0];
		stringLength = strlen("xoffset=");
		value = value.substr(stringLength);
		newFontData->xOffset = stoi(value);

		expression = "yoffset=-?\\d+";
		std::regex_search(fontFileLine, match, expression);
		value = match[0];
		stringLength = strlen("yoffset=");
		value = value.substr(stringLength);
		newFontData->yOffset = stoi(value);

		expression = "xadvance=-?\\d+";
		std::regex_search(fontFileLine, match, expression);
		value = match[0];
		stringLength = strlen("xadvance=");
		value = value.substr(stringLength);
		newFontData->xAdvance = stoi(value);
    
		m_glyphMap[charID] = newFontData;
	}

	// kearnings line
	std::getline(stream, fontFileLine);
	int kearningCount;
	expression = "kernings count=-?\\d+";
	std::regex_search(fontFileLine, match, expression);
	value = match[0];
	stringLength = strlen("kernings count=");
	value = value.substr(stringLength);
	kearningCount = stoi(value);

	//kearning lines
	for (int i = 0; i < kearningCount; i++)
	{
		std::getline(stream, fontFileLine);

		expression = "first=-?\\d+";
		std::regex_search(fontFileLine, match, expression);
		value = match[0];
		stringLength = strlen("first=");
		value = value.substr(stringLength);
		int firstID = stoi(value);

		expression = "second=-?\\d+";
		std::regex_search(fontFileLine, match, expression);
		value = match[0];
		stringLength = strlen("second=");
		value = value.substr(stringLength);
		int secondID = stoi(value);

		expression = "amount=-?\\d+";
		std::regex_search(fontFileLine, match, expression);
		value = match[0];
		stringLength = strlen("amount=");
		value = value.substr(stringLength);
		int amount = stoi(value);

		std::pair <int, int> idPair (firstID, secondID);
		m_kearningMap[idPair] = amount;
	}
}

float Font::CalculateTextWidth(char const *text, float scale /*= 1.f*/)
{
	float cursor = 0;

	char const *characterCode = text;
	int prevCharacterCode = -1;
	bool isCalculatingWidth = true;

	while (*characterCode != NULL)
	{
		if (*characterCode == '[')
			isCalculatingWidth = false;
		if (*characterCode == ']')
			isCalculatingWidth = true;

			t_FontData const *glyph = GetGlyph(*characterCode);

			if (glyph == nullptr)
			{
				glyph = GetInvalidGlyph();
			}

		prevCharacterCode = *characterCode;
		++characterCode;
		if (isCalculatingWidth)
		{
			cursor += glyph->xAdvance * scale;

			int kearning = GetKearning(prevCharacterCode, *characterCode);
			cursor += kearning * scale;
		}
	}

	return cursor;
}

float Font::CalculateTextHeight(char const *text, float scale /*= 1.f*/)
{
	float cursor = 0;

	char const *characterCode = text;
	int prevCharacterCode = -1;
	bool isCalculatingHeight = true;

	while (*characterCode != NULL)
	{
		if (*characterCode == '[')
			isCalculatingHeight = false;
		if (*characterCode == ']')
			isCalculatingHeight = true;

		t_FontData const *glyph = GetGlyph(*characterCode);

		if (glyph == nullptr)
		{
			glyph = GetInvalidGlyph();
		}

		prevCharacterCode = *characterCode;
		++characterCode;
		if (isCalculatingHeight)
		{
			float totalHeight = (glyph->height + glyph->yOffset) * scale;
			if (totalHeight > cursor)
				cursor = totalHeight;
		}
	}

	return cursor;
}

float Font::GetMaxTextHeight(char const *text, float scale /*= 1.f*/)
{
	float maxHeight = 0;

	char const *characterCode = text;

	while (*characterCode != NULL)
	{
		t_FontData const *glyph = GetGlyph(*characterCode);

		if (glyph == nullptr)
		{
			glyph = GetInvalidGlyph();
		}

		if (glyph->height * scale > maxHeight * scale)
		{
			maxHeight = glyph->height * scale;
		}

		++characterCode;
	}

	return maxHeight;
}

float Font::GetMaxYOffset(char const *text, float scale /*= 1.f*/)
{
	float maxOffset = 0;

	char const *characterCode = text;

	while (*characterCode != NULL)
	{
		t_FontData const *glyph = GetGlyph(*characterCode);

		if (glyph == nullptr)
		{
			glyph = GetInvalidGlyph();
		}

		if (glyph->yOffset * scale > maxOffset * scale)
		{
			maxOffset = glyph->yOffset * scale;
		}

		++characterCode;
	}

	return maxOffset;
}

float Font::GetMinYOffset(char const *text, float scale /*= 1.f*/)
{
	float minOffset = 0;

	char const *characterCode = text;

	while (*characterCode != NULL)
	{
		t_FontData const *glyph = GetGlyph(*characterCode);

		if (glyph == nullptr)
		{
			glyph = GetInvalidGlyph();
		}

		if (glyph->yOffset * scale < minOffset * scale)
		{
			minOffset = glyph->yOffset * scale;
		}

		++characterCode;
	}

	return minOffset;
}

t_FontData* Font::GetGlyph(int character)
{
	if (m_glyphMap.find(character) == m_glyphMap.end())
	{
		return nullptr;
	}
	else 
	{
		return m_glyphMap[character];
	}
}

t_FontData* Font::GetInvalidGlyph()
{
	return m_glyphMap[-1];
}

int Font::GetKearning(int prevChar, int nextChar)
{
	std::pair <int, int> idPair (prevChar, nextChar);
	return m_kearningMap[idPair];
}

void Font::CreateTexture(RHIDevice *device)
{
	m_texture = new Texture2D(device, m_textureFilePath.c_str());
}

void Font::Destroy()
{
	delete m_texture;
	m_texture = nullptr;

	std::map<int, t_FontData*> ::iterator glyphIter = m_glyphMap.begin();

	for (glyphIter; glyphIter != m_glyphMap.end(); ++glyphIter)
	{
		delete glyphIter->second;
	}
	m_glyphMap.clear();

	m_kearningMap.clear();
}
