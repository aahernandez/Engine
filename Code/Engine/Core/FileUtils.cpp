#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Memory/LogSystem.hpp"
#include "Engine/RHI/RHIOutput.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>

bool LoadBinaryFileToBuffer(const std::string& filePath, std::vector< unsigned char >& out_buffer)
{
	FILE* file = nullptr;
	errno_t errorCode = fopen_s(&file, filePath.c_str(), "rb");
	if (errorCode)
		return false;

	fseek(file, 0, SEEK_END);
	size_t numBytesToRead = ftell(file);
	rewind(file);

	out_buffer.resize(numBytesToRead);
	if (numBytesToRead > 0)
		fread(&out_buffer[0], sizeof(unsigned char), numBytesToRead, file);

	fclose(file);
	return true;
}

bool SaveBinaryFileFromBuffer(const std::string& filePath, const std::vector< unsigned char >& buffer)
{
	FILE* file = nullptr;
	errno_t errorCode = fopen_s(&file, filePath.c_str(), "wb");
	if (errorCode)
		return false;

	size_t numBytesToWrite = (int)buffer.size();
	if (numBytesToWrite > 0)
		fwrite(&buffer[0], sizeof(unsigned char), numBytesToWrite, file);

	fclose(file);
	return true;
}

bool SaveFileFromBuffer(const std::string& filePath, const std::vector< unsigned char >& buffer)
{
	FILE* file = nullptr;
	errno_t errorCode = fopen_s(&file, filePath.c_str(), "w");
	if (errorCode)
		return false;

	size_t numBytesToWrite = (int)buffer.size();
	if (numBytesToWrite > 0)
		fwrite(&buffer[0], sizeof(unsigned char), numBytesToWrite, file);

	fclose(file);
	return true;
}

bool LoadBinaryFileToBuffer(const std::string& filePath, std::vector< float >& out_buffer)
{
	FILE* file = nullptr;
	errno_t errorCode = fopen_s(&file, filePath.c_str(), "rb");
	if (errorCode)
		return false;

	fseek(file, 0, SEEK_END);
	size_t numBytesToRead = ftell(file);
	rewind(file);

	out_buffer.resize(numBytesToRead * sizeof(float));
	if (numBytesToRead > 0)
		fread(&out_buffer[0], sizeof(unsigned char), numBytesToRead, file);

	fclose(file);
	return true;
}

bool SaveBinaryFileFromBuffer(const std::string& filePath, const std::vector< float >& buffer)
{
	FILE* file = nullptr;
	errno_t errorCode = fopen_s(&file, filePath.c_str(), "wb");
	if (errorCode)
		return false;

	size_t numBytesToWrite = (int)buffer.size() * sizeof(float);
	if (numBytesToWrite > 0)
		fwrite(&buffer[0], sizeof(unsigned char), numBytesToWrite, file);

	fclose(file);
	return true;
}

bool LoadFileToBuffer(const std::string& filePath, std::vector< unsigned char >& out_buffer)
{
	FILE* file = nullptr;
	errno_t errorCode = fopen_s(&file, filePath.c_str(), "rt");
	if (errorCode)
		return false;

	fseek(file, 0, SEEK_END);
	size_t numBytesToRead = ftell(file);
	rewind(file);

	out_buffer.resize(numBytesToRead);
	if (numBytesToRead > 0)
		fread(&out_buffer[0], sizeof(unsigned char), numBytesToRead, file);

	fclose(file);
	return true;
}

bool LoadTextFileToStringMap(const std::string& filePath, std::map<std::string, std::string>& map)
{
	std::string configLine;
	std::ifstream myfile(filePath);

	if (myfile.is_open())
	{
		while (getline(myfile, configLine))
		{
			if (!configLine.empty() && configLine.at(0) != '#')
			{
				std::string variableName;
				std::string variableValue;
				std::size_t posOfFind = configLine.find("#");
				configLine = configLine.substr(0, posOfFind);

				posOfFind = configLine.find("+");
				std::size_t posOfFind2 = configLine.find("-");
				if (posOfFind != std::string::npos)
				{
					variableName = configLine.substr(posOfFind + 1);
					variableValue = "true";
				}
				else if (posOfFind2 != std::string::npos)
				{
					variableName = configLine.substr(posOfFind2 + 1);
					variableValue = "false";
				}
				else
				{
					posOfFind = configLine.find("=");
					variableName = configLine.substr(0, posOfFind);
					variableValue = configLine.substr(posOfFind + 1);

					posOfFind = variableName.find('"');
					if (posOfFind == std::string::npos)
					{
						variableName.erase(std::remove_if(variableName.begin(), variableName.end(), isspace), variableName.end());
					}
					else
					{
						variableName = variableName.substr(posOfFind + 1);
						posOfFind = variableName.find('"');
						variableName = variableName.substr(0, posOfFind);
					}
					posOfFind = variableValue.find('"');
					if (posOfFind == std::string::npos)
					{
						variableValue.erase(std::remove_if(variableValue.begin(), variableValue.end(), isspace), variableValue.end());
					}
					else
					{
						variableValue = variableValue.substr(posOfFind + 1);
						posOfFind = variableValue.find('"');
						variableValue = variableValue.substr(0, posOfFind);
					}
				}

				map[variableName] = variableValue;
			}
		}
		myfile.close();
	}
	else
		return false;

	return true;
}

void* FileReadToBuffer(char const *filename, size_t *out_size)
{
	*out_size = 0U;

	// I'm going to use Windows file functions for no particular reason
	// outside of familiarity.  Feel free to use whatever. 
	DWORD file_access = GENERIC_READ;
	DWORD share = FILE_SHARE_READ;
	DWORD options = OPEN_EXISTING;

	HANDLE file_handle = CreateFileA(filename, file_access, share, NULL, options, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file_handle == INVALID_HANDLE_VALUE) {
		DebuggerPrintf("File [%s] could not be opened.", filename);
		return nullptr;
	}

	// Get File Size
	DWORD high_word;
	DWORD lo_word = ::GetFileSize(file_handle, &high_word);

	// 64-bit way - use a 64-bit type, but we will never have a 4GB+ file, so ignore it and just using lo-word to prevent warnings
	// size_t size = ((size_t)high_word << 32) | (size_t)lo_word;
	size_t size = (size_t)lo_word;

	// Make a buffer that big
	void *buffer = malloc(size + 1U); // I allocae one extra for a null terminator - but do not include it in the size
									  // just in the cases I load strings it null terminates.
	if (nullptr != buffer) {
		DWORD read = 0U;
		ReadFile(file_handle, buffer, (DWORD)size, &read, nullptr);
		*out_size = read;

		((char*)buffer)[size] = NULL;
	}

	CloseHandle((HANDLE)file_handle);
	return buffer;
}

std::string GetFileNameWithoutPathAndFileType(const char *filename)
{
	std::vector<std::string> parsedString = ParseStringIntoPiecesByDelimiter(filename, "/");
	std::string newFilename = parsedString[parsedString.size() - 1];
	std::vector<std::string> parsedFilename = ParseStringIntoPiecesByDelimiter(newFilename, ".");
	return parsedFilename[0];
}

bool DoesFileExist(const char *filename)
{
	FILE* file = nullptr;
	errno_t errorCode = fopen_s(&file, filename, "rt");
	if (errorCode)
		return false;
	fclose(file);
	return true;
}

std::string OpenFileBrowserForFile(RHIOutput *rhiOutput, std::string startingFile)
{
	OPENFILENAME ofn;
	wchar_t szFile[260];
	ZeroMemory(&ofn, sizeof(ofn));
	if (startingFile != "")
	{
		size_t convertedChars = 0;
		mbstowcs_s(&convertedChars, szFile, startingFile.size(), startingFile.c_str(), _TRUNCATE);
		ofn.lpstrFile = szFile;
	}
	else
	{
		ofn.lpstrFile = szFile;
		ofn.lpstrFile[0] = '\0';
	}
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = rhiOutput->m_window->m_hwnd;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = _TEXT("All\0*.*\0Text\0*.TXT\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box. 

	if (GetOpenFileName(&ofn))
	{
		std::wstring ws(ofn.lpstrFile);
		return std::string(ws.begin(), ws.end());
	}
	else
	{
		return "";
	}
}

int ParseXmlAttribute(const ITCXMLNode& element, const char* attributeName, int defaultValue)
{
	const char* value = element.getAttribute(attributeName);
	if (value == NULL)
	{
		return defaultValue;
	}
	int returnValue = atoi(value);
	return returnValue;
}

std::string ParseXmlAttribute(const ITCXMLNode& element, const char* attributeName, const std::string& defaultValue)
{
	const char* value = element.getAttribute(attributeName);
	if (value == NULL)
	{
		return defaultValue;
	}
	std::string returnValue = value;
	return returnValue;
}

bool ParseXmlAttribute(const ITCXMLNode& element, const char* attributeName, bool defaultValue)
{
	const char* value = element.getAttribute(attributeName);
	if (value == NULL)
	{
		return defaultValue;
	}
	if (strcmp(value, "true") == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

char ParseXmlAttribute(const ITCXMLNode& element, const char* attributeName, char defaultValue)
{
	const char* value = element.getAttribute(attributeName);
	if (value == NULL)
	{
		return defaultValue;
	}
	char returnValue = *value;
	return returnValue;
}

RGBA ParseXmlAttribute(const ITCXMLNode& element, const char* attributeName, const RGBA& defaultValue)
{
	const char* value = element.getAttribute(attributeName);
	if (value == NULL)
	{
		return defaultValue;
	}
	std::string colorValue = value;

	std::vector<std::string> rgbValues = ParseStringIntoPiecesByDelimiter(colorValue, ", ");

	unsigned char rgbCharValues[3];
	for (int count = 0; count < 3; count++)
	{
		rgbCharValues[count] = (unsigned char)atoi(rgbValues[count].c_str());
	}

	return RGBA((unsigned char)rgbCharValues[0], rgbCharValues[1], rgbCharValues[2], 255);
}

float ParseXmlAttribute(const ITCXMLNode& element, const char* attributeName, float defaultValue)
{
	const char* value = element.getAttribute(attributeName);
	if (value == NULL)
	{
		return defaultValue;
	}
	float returnValue = std::stof(value);
	return returnValue;
}

Vector2 ParseXmlAttribute(const ITCXMLNode& element, const char* attributeName, const Vector2& defaultValue)
{
	const char* value = element.getAttribute(attributeName);
	if (value == NULL)
	{
		return defaultValue;
	}
	std::string vectorValue = value;

	std::vector<std::string> floats = ParseStringIntoPiecesByDelimiter(vectorValue, ", ");

	float floatValues[2];
	for (int count = 0; count < 2; count++)
	{
		floatValues[count] = std::stof(floats[count]);
	}

	return Vector2(floatValues[0], floatValues[1]);
}

IntVector2 ParseXmlAttribute(const ITCXMLNode& element, const char* attributeName, const IntVector2& defaultValue)
{
	const char* value = element.getAttribute(attributeName);
	if (value == NULL)
	{
		return defaultValue;
	}
	std::string vectorValue = value;

	std::vector<std::string> ints = ParseStringIntoPiecesByDelimiter(vectorValue, ",");

	int intValues[2];
	for (int count = 0; count < 2; count++)
	{
		intValues[count] = atoi(ints[count].c_str());
	}

	return IntVector2(intValues[0], intValues[1]);
}

IntRange ParseXmlAttribute(const ITCXMLNode& element, const char* attributeName, const IntRange& defaultRange)
{
	const char* value = element.getAttribute(attributeName);
	if (value == NULL)
	{
		return defaultRange;
	}
	std::string vectorValue = value;

	std::vector<std::string> ints = ParseStringIntoPiecesByDelimiter(vectorValue, ",");

	int intValues[2];
	for (int count = 0; count < 2; count++)
	{
		intValues[count] = atoi(ints[count].c_str());
	}

	return IntRange(intValues[0], intValues[1]);
}

Range ParseXmlAttribute(const ITCXMLNode& element, const char* attributeName, const Range& defaultRange)
{
	const char* value = element.getAttribute(attributeName);
	if (value == NULL)
	{
		return defaultRange;
	}
	std::string vectorValue = value;

	std::vector<std::string> floats = ParseStringIntoPiecesByDelimiter(vectorValue, ",");

	float floatValues[2];
	for (int count = 0; count < 2; count++)
	{
		floatValues[count] = std::stof(floats[count]);
	}

	return Range(floatValues[0], floatValues[1]);
}

Trilean ParseXmlAttribute(const ITCXMLNode& element, const char* attributeName, Trilean defaultValue)
{
	const char* value = element.getAttribute(attributeName);
	if (value == NULL)
	{
		return defaultValue;
	}

	if (strcmp(value, "true") == 0)
	{
		return TRI_TRUE;
	}
	else if (strcmp(value, "false") == 0)
	{
		return TRI_FALSE;
	}
	else
	{
		return TRI_NONE;
	}
}

byte ParseXmlAttribute(const ITCXMLNode& element, const char* attributeName, byte defaultValue)
{
	const char* value = element.getAttribute(attributeName);
	if (value == NULL)
	{
		return defaultValue;
	}
	int returnValue = atoi(value);
	return (byte)returnValue;
}

std::string ParseXmlChildElementText(const ITCXMLNode& element, const char* childNodeName, const std::string& defaultValue)
{
	ITCXMLNode childElement = element.getChildNode(childNodeName);
	if (childElement.nText() != 0)
	{
		return childElement.getText();
	}
	else
	{
		return defaultValue;
	}
}

byte ParseXmlChildElementText(const ITCXMLNode& element, const char* childNodeName, const byte& defaultValue)
{
	ITCXMLNode childElement = element.getChildNode(childNodeName);
	if (childElement.nText() != 0)
	{
		return (byte)atoi(childElement.getText());
	}
	else
	{
		return defaultValue;
	}
}

void ValidateXmlElement(const ITCXMLNode& element, std::string commaSeparatedListOfValidChildElementNames, std::string commaSeparatedListOfValidAttributeNames)
{
	std::vector<std::string> validChildElementNames = ParseStringIntoPiecesByDelimiter(commaSeparatedListOfValidChildElementNames, ",");
	std::vector<std::string> validAttributeNames = ParseStringIntoPiecesByDelimiter(commaSeparatedListOfValidAttributeNames, ",");

	int numElements = element.nChildNode();
	for (int childElementCount = 0; childElementCount < numElements; childElementCount++)
	{
		bool isValid = false;
		ITCXMLNode childElement = element.getChildNode(childElementCount);

		std::vector<std::string>::iterator iter = validChildElementNames.begin();
		for (iter; iter != validChildElementNames.end(); ++iter)
		{
			if (std::strcmp(iter->c_str(), childElement.getName()) == 0)
			{
				isValid = true;
				break;
			}
		}
		ASSERT_OR_DIE(isValid, Stringf("An invalid child element \"%s\" was found under \"%s\"", childElement.getName(), element.getName()));
	}

	int numAttributes = element.nAttribute();
	for (int attributeCount = 0; attributeCount < numAttributes; attributeCount++)
	{
		bool isValid = false;
		std::string attributeName = element.getAttributeName(attributeCount);

		std::vector<std::string>::iterator iter = validAttributeNames.begin();
		for (iter; iter != validAttributeNames.end(); ++iter)
		{
			if (std::strcmp(iter->c_str(), attributeName.c_str()) == 0)
			{
				isValid = true;
				break;
			}
		}
		ASSERT_OR_DIE(isValid, Stringf("An invalid attribute name \"%s\" was found in \"%s\"", attributeName.c_str(), element.getName()));
	}
}

void StoreXmlChildNodeAttributes(const ITCXMLNode& element, const char* childNodeName, const char* attributeName, std::vector<RGBA>& RGBAs)
{
	unsigned int numChildNodes = element.nChildNode(childNodeName);

	for (unsigned int childNodeCount = 0; childNodeCount < numChildNodes; childNodeCount++)
	{
		ITCXMLNode childNode = element.getChildNode(childNodeName, childNodeCount);
		RGBAs.push_back(ParseXmlAttribute(childNode, attributeName, RGBA::PINK));
	}
}

void StoreXmlChildNodeAttributes(const ITCXMLNode& element, const char* childNodeName, const char* attributeName, std::vector<char>& chars)
{
	unsigned int numChildNodes = element.nChildNode(childNodeName);

	for (unsigned int childNodeCount = 0; childNodeCount < numChildNodes; childNodeCount++)
	{
		ITCXMLNode childNode = element.getChildNode(childNodeName, childNodeCount);
		chars.push_back(ParseXmlAttribute(childNode, attributeName, '\0'));
	}
}

void StoreXmlChildNodeAttribute(const ITCXMLNode& element, const char* childNodeName, const char* attributeName, std::string& stringToChange)
{
	ITCXMLNode childNode = element.getChildNode(childNodeName);
	stringToChange = ParseXmlAttribute(childNode, attributeName, std::string(""));
}
