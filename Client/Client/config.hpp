#pragma once
#include <string>


class config
{
private:

	static bool m_isInitialised;
	static int m_currentWindow;
	static int m_currentLanguage;

	static void Initialise(const std::string& _txt, bool& _isSuccessful);
	static bool InitFolders();

public:
	static std::string NormaliseDir(std::string& _str);

	static bool StartConfigs();

	static bool UpdateLanguage(const int _language);

	static void SetIp(std::string& _ip);

	static void SetPort(std::string& _port);

	static void ChangeWindow(const int _window);

	

	// For requesting a chatroom with a specific domain. Not to be confused with the actual domain server ip/port
	static std::string SDomainIp;
	static std::wstring WDomainIp;
	static std::string SDomainPort;
	static int IDomainPort;

	static std::string currentDirNormalised;
	static std::string currentDirUnNormalised;

};

