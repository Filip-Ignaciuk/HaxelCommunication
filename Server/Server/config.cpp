#include <string>
#include <filesystem>
#include <fstream>

#include "config.hpp"
#include "ErrorHandler.hpp"
#include "Languages.hpp"

bool config::m_isInitialised = false;
int config::m_currentWindow = 0;
int config::m_currentLanguage = 0;

Chatroom config::m_chatroom;

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

void config::InitFolders()
{
	if (!std::filesystem::create_directory(currentDirNormalised + "/Languages"))
	{
		Error FileSystemError("Failed to create folders for language files.", 0);
		ErrorHandler::AddError(FileSystemError);
	}

	if (!std::filesystem::create_directory(currentDirNormalised + "/Users"))
	{
		Error FileSystemError("Failed to create folders for user files.", 0);
		ErrorHandler::AddError(FileSystemError);
	}

}

void config::Initialise(const std::string& _txt)
{
	std::ofstream file(_txt);
	file << "1" << std::endl; // Is Application initialised
	file << "0" << std::endl; // Language
	file << "1" << std::endl; // TimeFormat
	file << SDomainIp << std::endl; // Domain Ip
	file << SDomainPort << std::endl; // Domain Port

	m_currentLanguage = 0;
	//isTimeFormatOn = true;

	// Initialising folders.
	InitFolders();

	// Initialising Language File.
	file.close();
	LanguageFileInitialiser::GenerateLanguageFile(0);
	LanguageFileInitialiser::PopulateAllTextsInApplication();
	m_isInitialised = 1;
}

void config::UpdateLanguage(const int _language)
{
	LanguageFileInitialiser::ChangeLanguage(_language);
}

void config::StartConfigs()
{
	
	const std::string initialisedTxt = currentDirNormalised + "/initialised.txt";
	std::ifstream initFile(initialisedTxt);
	if (!initFile.is_open())
	{
		initFile.close();
		Initialise(initialisedTxt);
	}
	else
	{
		LanguageFileInitialiser::CheckInstalledLanguages();
		std::string line;
		int i = 0;
		while (std::getline(initFile, line))
		{
			if (i == 0) { m_isInitialised = std::stoi(line); } // Is Application initialised
			else if (i == 1) { m_currentLanguage = std::stoi(line); } // Language
			//else if (i == 2) { isTimeFormatOn = std::stoi(line); } // TimeFormat
			else if (i == 3) { SetIp(line); } // Domain Ip
			else if (i == 4) { SetPort(line); } // Domain Port
			
 
			i++;
		}
		initFile.close();

		if (!LanguageFileInitialiser::initialisedLanguages[m_currentLanguage])
		{
			LanguageFileInitialiser::GenerateLanguageFile(m_currentLanguage);
		}

		LanguageFileInitialiser::PopulateAllTextsInApplication();

		
	}


}

void config::SetIp(std::string& _ip)
{
	SDomainIp = _ip;
	WDomainIp = std::wstring(SDomainIp.begin(), SDomainIp.end());
}

void config::SetPort(std::string& _port)
{
	SDomainPort = _port;
	IDomainPort = std::stoi(SDomainPort);
}

void config::ChangeWindow(const int _window)
{
	m_currentWindow = _window;
}

bool config::GetIsInitialised()
{
	return m_isInitialised;
}

int config::GetCurrentWindow()
{
	return m_currentWindow;
}

int config::GetCurrentLanguage()
{
	return m_currentLanguage;
}

void config::SetChatroom(Chatroom& _chatroom)
{
	m_chatroom = _chatroom;
}

Chatroom& config::GetChatroom()
{
	return m_chatroom;
}


// Domain Config
std::string config::SDomainIp = "127.0.0.1";
std::wstring config::WDomainIp = std::wstring(config::SDomainIp.begin(), SDomainIp.end());
std::string config::SDomainPort = "4096";
int config::IDomainPort = 4096;

std::string config::currentDirUnNormalised = std::filesystem::current_path().string();
std::string config::currentDirNormalised = NormaliseDir(currentDirUnNormalised);
