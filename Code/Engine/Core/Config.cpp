#include "Config.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

Config::Config(char const *filePath)
{
	ConfigSystemStartup(filePath);
}

Config::~Config()
{
	ConfigSystemShutdown();
}

void Config::ConfigSystemStartup(char const *filePath)
{
	ConfigLoadFile(filePath);
	ConfigList();
}

void Config::ConfigSystemShutdown()
{
// 	std::map<std::string, std::string>::iterator mapIter;
// 	for (mapIter = m_configMap.begin(); mapIter != m_configMap.end(); ++mapIter)
// 	{
// 		mapIter = m_configMap.erase(mapIter);
// 		if (mapIter == m_configMap.end())
// 			break;
// 	}
// 	m_configMap.clear();
}

void Config::ConfigSet(char const *name, char const *value)
{
	m_configMap[name] = value;
}

void Config::ConfigSet(char const *name, int const value)
{
	m_configMap[name] = std::to_string(value);
}

void Config::ConfigSet(char const *name, float const value)
{
	m_configMap[name] = std::to_string(value);
}

void Config::ConfigSet(char const *name, bool const value)
{
	if (value == true)
		m_configMap[name] = "true";
	else
		m_configMap[name] = "false";

}

bool Config::IsConfigSet(char const *id)
{
	if (m_configMap.find(id) == m_configMap.end()) {
		return false;
	}
	else {
		return true;
	}
}

void Config::ConfigUnset(char const *id)
{
	m_configMap.erase(id);
}

bool Config::ConfigGetString(char const** outv, char const *id)
{
	*outv = m_configMap[id].c_str();
	return true;
}

bool Config::ConfigGetBool(bool *outv, char const *id)
{
	if (m_configMap[id].compare("true") == 0)
	{
		*outv = true;
		return true;
	}
	else if (m_configMap[id].compare("false") == 0)
	{
		*outv = false;
		return true;
	}
	else 
		return false;
}

bool Config::ConfigGetInt(int *outv, char const *id)
{
	*outv = atoi(m_configMap[id].c_str());
	if (*outv == 0)
		return false;
	return true;
}

bool Config::ConfigGetFloat(float *outv, char const *id)
{
	*outv = (float)atof(m_configMap[id].c_str());
	if (*outv == 0.f)
		return false;
	return true;
}

void Config::ConfigList()
{
	DebuggerPrintf("CONFIG LIST");
	std::map<std::string, std::string>::iterator configIter;
	for (configIter = m_configMap.begin(); configIter != m_configMap.end(); configIter++) 
	{
		DebuggerPrintf("%s = %s", configIter->first.c_str(), configIter->second.c_str());
	}
}

bool Config::ConfigLoadFile(char const *filename)
{
	bool wasLoaded = LoadTextFileToStringMap(filename, m_configMap);
	return wasLoaded;
}
