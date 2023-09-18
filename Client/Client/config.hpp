#pragma once
#include <string>

class config
{
private:

	

	static bool InitFolders();

public:

	static std::string NormaliseDir(std::string& _str);

	static bool StartConfigs();
	static bool UpdateLanguage(const int _language);

	static std::string currentDirNormalised;
	static std::string currentDirUnNormalised;

	static bool isInitialised;

	static int currentLanguage;

	static bool isTimeFormatOn;
};

