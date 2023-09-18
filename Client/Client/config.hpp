#pragma once
#include <string>

class DomainServerConfig
{
private:
	static std::string ip;
	static std::string port;
public:
	static std::string GetIp() { return ip; } 
	static std::string SetIp(std::string& _ip) { ip = _ip; }
	static std::string GetPort() { return port; }
	static std::string SetPort(std::string& _port) { port = _port; }
};

class config
{
private:

	

	static bool InitFolders();

public:

	static DomainServerConfig DSC;

	static std::string NormaliseDir(std::string& _str);

	static bool StartConfigs();
	static bool UpdateLanguage(const int _language);

	static std::string currentDirNormalised;
	static std::string currentDirUnNormalised;

	static bool isInitialised;

	static int currentLanguage;

	static bool isTimeFormatOn;
};

