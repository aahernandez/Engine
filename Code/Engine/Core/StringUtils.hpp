//Code written by Squirrel Eiserloh
#pragma once
//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... );
const std::string Stringf( const char* format, va_list variableArguments );
const std::string Stringf( const int maxLength, const char* format, ... );

void MakeStringLowerCase(std::string& string);
int ConvertNumWordToInt(std::string numString);
void RemoveCharacterFromString(std::string& stringToParse, char characterToRemove);
void RemoveSubstring(std::string& stringToParse, const std::string& wordToRemove);
bool DoesSubstringExistInString(const std::string& stringToCheck, const std::string& substring);
int GetSubstringLocationInString(const std::string& stringToCheck, const std::string& substring);
std::vector<std::string> ParseStringIntoPiecesByDelimiter(std::string stringToParse, std::string delimiter);
std::vector<int> ParseStringIntoIntPiecesByDelimiter(std::string stringToParse, std::string delimiter);




