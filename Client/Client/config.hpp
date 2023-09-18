#pragma once
#include <string>

static class config
{
private:

	static std::string NormaliseDir(std::string& _str);

	static bool InitFolders();

public:
	static bool StartConfigs();
	static bool UpdateLanguage(const int _language);

	static std::string currentDirNormalised;

	static int currentLanguage;

	static bool isTimeFormatOn;
};

