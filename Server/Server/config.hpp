#pragma once
#include <string>

#include "Chatroom.hpp"


class config
{
private:

	static bool m_isInitialised;
	static int m_currentLanguage;

	static void Initialise(const std::string& _txt);
	static void InitFolders();

public:
	static std::string NormaliseDir(std::string& _str);

	static void StartConfigs();

	static bool GetIsInitialised();

	static void UpdateLanguage(int _language);
	static int GetCurrentLanguage();



	static std::string currentDirNormalised;
	static std::string currentDirUnNormalised;

};

