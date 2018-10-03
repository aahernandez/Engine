#pragma once
#include "Engine/Core/RGBA.hpp"
#include "Engine/Core/CommonInclude.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/Range.hpp"
#include "ThirdParty/IXMLParser/IXMLParser.h"
#include <string>
#include <vector>
#include <map>

class RHIOutput;

bool LoadBinaryFileToBuffer(const std::string& filePath, std::vector< unsigned char >& out_buffer);
bool SaveBinaryFileFromBuffer(const std::string& filePath, const std::vector< unsigned char >& buffer);
bool LoadBinaryFileToBuffer(const std::string& filePath, std::vector< float >& out_buffer);
bool SaveBinaryFileFromBuffer(const std::string& filePath, const std::vector< float >& buffer);
bool LoadFileToBuffer(const std::string& filePath, std::vector< unsigned char >& out_buffer);
bool SaveFileFromBuffer(const std::string& filePath, const std::vector< unsigned char >& buffer);

bool LoadTextFileToStringMap(const std::string& filePath, std::map<std::string, std::string>& map);
void* FileReadToBuffer(char const *filename, size_t *out_size);

std::string GetFileNameWithoutPathAndFileType(const char *filename);
bool DoesFileExist(const char *filename);

std::string OpenFileBrowserForFile(RHIOutput *ownderWindow, std::string startingFile = "");

int ParseXmlAttribute(const ITCXMLNode& element, const char* attributeName, int defaultValue);
byte ParseXmlAttribute(const ITCXMLNode& element, const char* attributeName, byte defaultValue);
char ParseXmlAttribute(const ITCXMLNode& element, const char* attributeName, char defaultValue);
bool ParseXmlAttribute(const ITCXMLNode& element, const char* attributeName, bool defaultValue);
float ParseXmlAttribute(const ITCXMLNode& element, const char* attributeName, float defaultValue);
Trilean ParseXmlAttribute(const ITCXMLNode& element, const char* attributeName, Trilean defaultValue);
RGBA ParseXmlAttribute(const ITCXMLNode& element, const char* attributeName, const RGBA& defaultValue);
Vector2 ParseXmlAttribute(const ITCXMLNode& element, const char* attributeName, const Vector2& defaultValue);
Range ParseXmlAttribute(const ITCXMLNode& element, const char* attributeName, const Range& defaultRange);
IntRange ParseXmlAttribute(const ITCXMLNode& element, const char* attributeName, const IntRange& defaultRange);
IntVector2 ParseXmlAttribute(const ITCXMLNode& element, const char* attributeName, const IntVector2& defaultValue);
std::string ParseXmlAttribute(const ITCXMLNode& element, const char* attributeName, const std::string& defaultValue);

std::string ParseXmlChildElementText(const ITCXMLNode& element, const char* childNodeName, const std::string& defaultValue);
byte ParseXmlChildElementText(const ITCXMLNode& element, const char* childNodeName, const byte& defaultValue);

void ValidateXmlElement(const ITCXMLNode& element, std::string commaSeparatedListOfValidChildElementNames, std::string commaSeparatedListOfValidAttributeNames);

void StoreXmlChildNodeAttributes(const ITCXMLNode& element, const char* childNodeName, const char* attributeName, std::vector<RGBA>& RGBAs);
void StoreXmlChildNodeAttributes(const ITCXMLNode& element, const char* childNodeName, const char* attributeName, std::vector<char>& chars);
void StoreXmlChildNodeAttribute(const ITCXMLNode& element, const char* childNodeName, const char* attributeName, std::string& stringToChange);
