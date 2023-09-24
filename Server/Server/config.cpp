#include <string>
#include <filesystem>
#include <fstream>

#include "config.hpp"

#include <WinSock2.h>
#include <ws2def.h>
#include <WS2tcpip.h>


std::string config::NormaliseDir(std::string& _str)
{
	// Converting all the \\ slashes into one singular forward slash.
	for (unsigned int i = 0; i < _str.size(); i++)
	{
		if (_str[i] == '\\')
		{
			_str[i] = '/';
		}
	}
	return _str;
}

bool config::InitFolders()
{
	if (std::filesystem::create_directory(currentDirNormalised + "/Servers") == 0)
	{
		return false;
	}

	return true;
}

void config::Initialise(const std::string& _txt, bool& _isSuccessful)
{
	std::ofstream file(_txt);
	// Initialising folders.
	_isSuccessful = InitFolders();
	if (_isSuccessful)
	{
		file << "1" << std::endl; // Is Application initialised
	}
	else
	{
		file << "0" << std::endl; // Is Application initialised
	}
	file << SDomainPort << std::endl; // Domain Ip
	file << SDomainIp << std::endl; // Domain Port
	file.close();
}





bool config::StartConfigs()
{
	bool isSuccessful = true;

	const std::string initialisedTxt = currentDirNormalised + "/initialised.txt";
	std::ifstream initFile(initialisedTxt);
	if (!initFile.is_open())
	{
		initFile.close();
		Initialise(initialisedTxt, isSuccessful);


	}
	else
	{
		std::string line;
		int i = 0;
		while (std::getline(initFile, line))
		{
			if (i == 0) { isInitialised = std::stoi(line); } // Is Application initialised
			else if (i == 3) { SetIp(line); } // Domain Ip
			else if (i == 4) { SetPort(line); } // Domain Port


			i++;
		}
		initFile.close();

		if(!isInitialised)
		{
			Initialise(initialisedTxt, isSuccessful);
		}
	}

	return isSuccessful;




}

void config::SetIp(std::string& _ip)
{
	SDomainIp = _ip;
	WDomainIp = std::wstring(config::SDomainIp.begin(), SDomainIp.end());
}

void config::SetPort(std::string& _port)
{
	SDomainPort = _port;
	IDomainPort = std::stoi(SDomainPort);
}

bool config::IsIpValid(std::string& _ip)
{
	int dotCount = 0;
	for (unsigned char i = 0; i < _ip.size(); i++)
	{
		if (_ip[i] == '.')
		{
			dotCount++;
		}
		else if (!isdigit(_ip[i]))
		{
			return false;
		}

	}
	if (dotCount != 3)
	{
		return false;
	}
	return true;
}

bool config::IsPortValid(std::string& _port)
{
	const int port = std::stoi(_port);
	const int portSize = _port.size();
	if (port < 1024)
	{
		return false;
	}
	if (port > 49151)
	{
		return false;
	}
	if(portSize < 4 || portSize > 5)
	{
		return false;
	}
	for (unsigned char i = 0; i < portSize; i++)
	{
		if (!isdigit(_port[i]))
		{
			return false;
		}

	}
	return true;
}

bool config::StartWinsock()
{
	SOCKET serverSocket = INVALID_SOCKET;
	WSADATA wsaData;
	WORD version = MAKEWORD(2, 2);
	if (WSAStartup(version, &wsaData))
	{
		currentError = "Winsock DLL failed to be found/loaded. Error: " + WSAGetLastError();
		return false;
	}

	allServerText.emplace_back("DLL has been successfully found/loaded.");

	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == INVALID_SOCKET)
	{
		currentError = "Error creating server socket.";
		WSACleanup();
	}

	allServerText.emplace_back("Server socket has been successfully created.");
}

void config::StartServer()
{
	// converting ip input to const wchar_t
	std::wstring wideIpInput = std::wstring(serverIp.begin(), serverIp.end());
	PCWSTR ip = wideIpInput.c_str();
	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_port = htons(std::stoi(serverPort));
	InetPtonW(AF_INET, ip, &service.sin_addr.S_un.S_addr);
	if (bind(serverSocket, reinterpret_cast<SOCKADDR*>(&service), sizeof(service)))
	{
		currentError = "Binding failed, perhaps an invalid or non-existent ip";
	}
	else
	{
		isServerInitialised = true;
		allServerText.emplace_back("Server socket bind has been successfully done.");
	}
}

std::string config::currentError = "None";

std::vector<std::string> config::allServerText;

// Domain Config
std::string config::SDomainIp = "127.0.0.1";
std::wstring config::WDomainIp = std::wstring(config::SDomainIp.begin(), SDomainIp.end());
std::string config::SDomainPort = "4096";
int config::IDomainPort = 4096;

// Server Config
SOCKET config::serverSocket = SOCKET_ERROR;
std::string config::serverIp = "127.0.0.1";
std::string config::serverPort = "8102";

std::string config::currentDirUnNormalised = std::filesystem::current_path().string();
std::string config::currentDirNormalised = NormaliseDir(currentDirUnNormalised);

bool config::isInitialised = false;
bool config::isServerInitialised = false;

