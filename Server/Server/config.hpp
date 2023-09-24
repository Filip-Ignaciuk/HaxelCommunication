#pragma once
#include <string>


class config
{
private:
	static void Initialise(const std::string& _txt, bool& _isSuccessful);
	static bool InitFolders();

public:
	static std::string NormaliseDir(std::string& _str);

	static bool StartConfigs();

	static void SetIp(std::string& _ip);

	static void SetPort(std::string& _port);

	static std::string SDomainIp;
	static std::wstring WDomainIp;
	static std::string SDomainPort;
	static int IDomainPort;

	static std::string currentDirNormalised;
	static std::string currentDirUnNormalised;

	static bool isInitialised;

};

