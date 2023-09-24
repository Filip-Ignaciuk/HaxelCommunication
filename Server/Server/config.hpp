#pragma once
#include <string>
#include <WinSock2.h>


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

	static bool IsIpValid(std::string& _ip);

	static bool IsPortValid(std::string& _port);

	static bool StartWinsock();

	static void StartServer();

	static void AddToErrorLog(std::string& _message);


	static std::string currentError;

	static std::vector<std::string> allServerText;


	// For requesting a chatroom with a specific domain. Not to be confused with the actual domain server ip/port
	static std::string SDomainIp;
	static std::wstring WDomainIp;
	static std::string SDomainPort;
	static int IDomainPort;

	// Actual server
	static SOCKET serverSocket;
	static std::string serverIp;
	static std::string serverPort;

	static std::string currentDirNormalised;
	static std::string currentDirUnNormalised;

	static bool isInitialised;
	static bool isServerInitialised;



};

