#pragma once
#include "Engine/Core/FileUtils.hpp"

class Config
{
public:
	Config(char const *filePath);
	~Config();

	void ConfigSystemStartup(char const *filePath);
	void ConfigSystemShutdown();

	void ConfigSet(char const *name, char const *value);
	void ConfigSet(char const *name, int const value);
	void ConfigSet(char const *name, float const value);
	void ConfigSet(char const *name, bool const value);

	bool IsConfigSet(char const *id);
	void ConfigUnset(char const *id);

	bool ConfigGetString(char const** outv, char const *id);
	bool ConfigGetBool(bool *outv, char const *id);
	bool ConfigGetInt(int *outv, char const *id);
	bool ConfigGetFloat(float *outv, char const *id);

	void ConfigList();

	bool ConfigLoadFile(char const *filename);

private:
	std::map<std::string, std::string> m_configMap;
};