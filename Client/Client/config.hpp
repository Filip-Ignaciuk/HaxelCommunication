#pragma once
#include <string>


class config
{
private:
	static std::string SDomainIp;
	static std::wstring WDomainIp;
	static std::string SDomainPort;
	static int IDomainPort;

	static void Initialise(const std::string& _txt, bool& _isSuccessful);
	static bool InitFolders();

public:
	static std::string NormaliseDir(std::string& _str);

	static bool StartConfigs();

	static bool UpdateLanguage(const int _language);

	static void SetIp(std::string& _ip)
	{
		SDomainIp = _ip;
		WDomainIp = std::wstring(config::SDomainIp.begin(), SDomainIp.end());
	}

	static std::string GetSIp()
	{
		return SDomainIp;
	}

	static std::wstring GetWIp()
	{
		return WDomainIp;
	}

	static void SetPort(std::string& _port)
	{
		SDomainPort = _port;
		IDomainPort = std::stoi(SDomainPort);
	}

	static std::string GetSPort()
	{
		return SDomainPort;
	}

	static int GetIPort()
	{
		return IDomainPort;
	}

	static std::string currentDirNormalised;
	static std::string currentDirUnNormalised;

	static bool isInitialised;

	static int currentLanguage;

	static bool isTimeFormatOn;
};

