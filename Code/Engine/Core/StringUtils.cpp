//Code written by Squirrel Eiserloh
#include "Engine/Core/StringUtils.hpp"
#include <algorithm>
#include <stdarg.h>


//-----------------------------------------------------------------------------------------------
const int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... )
{
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	return std::string( textLiteral );
}

const std::string Stringf(const char* format, va_list variableArguments)
{
	char textLiteral[STRINGF_STACK_LOCAL_TEMP_LENGTH];
	vsnprintf_s(textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArguments);
	textLiteral[STRINGF_STACK_LOCAL_TEMP_LENGTH - 1] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	return std::string(textLiteral);
}


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const int maxLength, const char* format, ... )
{
	char textLiteralSmall[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	char* textLiteral = textLiteralSmall;
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		textLiteral = new char[ maxLength ];

	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, maxLength, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ maxLength - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	std::string returnValue( textLiteral );
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		delete[] textLiteral;

	return returnValue;
}

void MakeStringLowerCase(std::string& string)
{
	std::transform(string.begin(), string.end(), string.begin(), ::tolower);
}

int ConvertNumWordToInt(std::string numString)
{
	std::string numStringCopy = numString;
	std::transform(numStringCopy.begin(), numStringCopy.end(), numStringCopy.begin(), ::tolower);
	if (numStringCopy == "one" || numStringCopy == "once" || numStringCopy == "a")
	{
		return 1;
	}
	else if (numStringCopy == "two" || numStringCopy == "twice")
	{
		return 2;
	}
	else if (numStringCopy == "three")
	{
		return 3;
	}
	else if (numStringCopy == "four")
	{
		return 4;
	}
	else if (numStringCopy == "five")
	{
		return 5;
	}
	else if (numStringCopy == "six")
	{
		return 6;
	}
	else if (numStringCopy == "seven")
	{
		return 7;
	}
	else if (numStringCopy == "eight")
	{
		return 8;
	}
	else if (numStringCopy == "nine")
	{
		return 9;
	}
	else if (numStringCopy == "ten")
	{
		return 10;
	}
	return -1;
}

void RemoveCharacterFromString(std::string& stringToParse, char characterToRemove)
{
	std::string::iterator stringIter = std::find(stringToParse.begin(), stringToParse.end(), characterToRemove);
	while (stringIter != stringToParse.end())
	{
		stringToParse.erase(stringIter);
		stringIter = std::find(stringToParse.begin(), stringToParse.end(), characterToRemove);
	}
}

void RemoveSubstring(std::string& stringToParse, const std::string& wordToRemove)
{
	std::size_t substringPosition = stringToParse.find(wordToRemove);
	while (substringPosition != std::string::npos)
	{
		stringToParse.replace(substringPosition, wordToRemove.length(), "");
		substringPosition = stringToParse.find(wordToRemove);
	}
}

bool DoesSubstringExistInString(const std::string& stringToCheck, const std::string& substring)
{
	std::size_t substringPosition = stringToCheck.find(substring);
	if (substringPosition != std::string::npos)
	{
		return true;
	}
	return false;
}

int GetSubstringLocationInString(const std::string& stringToCheck, const std::string& substring)
{
	std::size_t substringPosition = stringToCheck.find(substring);
	if (substringPosition != std::string::npos)
	{
		return (int)substringPosition;
	}
	return -1;
}

std::vector<std::string> ParseStringIntoPiecesByDelimiter(std::string stringToParse, std::string delimiter)
{
	std::vector<std::string> parsedString;
	while (!stringToParse.empty())
	{
		size_t positionOfDelimiter = stringToParse.find(delimiter);
		std::string cutString = stringToParse.substr(0, positionOfDelimiter);
		parsedString.push_back(cutString);
		if (positionOfDelimiter > 1000)
			break;
		stringToParse = stringToParse.substr(positionOfDelimiter + delimiter.length());
	}

	return parsedString;
}

std::vector<int> ParseStringIntoIntPiecesByDelimiter(std::string stringToParse, std::string delimiter)
{
	std::vector<int> parsedString;

	while (!stringToParse.empty())
	{
		size_t positionOfDelimiter = stringToParse.find(delimiter);
		std::string cutString = stringToParse.substr(0, positionOfDelimiter);
		RemoveCharacterFromString(cutString, ' ');
		if (cutString == "")
		{
			parsedString.push_back(0);
		}
		else
		{
			parsedString.push_back(stoi(cutString));
		}

		if (positionOfDelimiter > 1000)
			break;
		stringToParse = stringToParse.substr(positionOfDelimiter + delimiter.length());
	}

	return parsedString;
}



