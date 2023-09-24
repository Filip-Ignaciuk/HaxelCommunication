#include <string>
#include <filesystem>
#include <fstream>

#include "config.hpp"


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


// Domain Config
std::string config::SDomainIp = "127.0.0.1";
std::wstring config::WDomainIp = std::wstring(config::SDomainIp.begin(), SDomainIp.end());
std::string config::SDomainPort = "4096";
int config::IDomainPort = 4096;

std::string config::currentDirUnNormalised = std::filesystem::current_path().string();
std::string config::currentDirNormalised = NormaliseDir(currentDirUnNormalised);

bool config::isInitialised = false;

