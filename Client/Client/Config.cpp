#include <string>
#include <filesystem>
#include <fstream>

#include "config.hpp"
#include "Languages.hpp"

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
	if (std::filesystem::create_directory(currentDirNormalised + "/Languages") == 0)
	{
		return false;
	}

	if (std::filesystem::create_directory(currentDirNormalised + "/Users") == 0)
	{
		return false;
	}

	return true;
}

void config::Initialise(const std::string& _txt, bool& _isSuccessful)
{
	std::ofstream file(_txt);
	file << "1" << std::endl; // Is Application initialised
	file << "0" << std::endl; // Language
	file << "1" << std::endl; // TimeFormat
	file << GetSIp() << std::endl; // Domain Ip
	file << GetSPort() << std::endl; // Domain Port

	currentLanguage = 0;
	isTimeFormatOn = true;

	// Initialising folders.
	_isSuccessful = InitFolders();

	// Initialising Language File.
	file.close();
	LanguageFileInitialiser::GenerateLanguageFile(0);
	LanguageFileInitialiser::PopulateAllTextsInApplication();
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
		LanguageFileInitialiser::CheckInstalledLanguages();
		std::string line;
		int i = 0;
		while (std::getline(initFile, line))
		{
			if (i == 0) {  isInitialised = std::stoi(line); } // Is Application initialised
			else if (i == 1) { currentLanguage = std::stoi(line); } // Language
			else if (i == 2) { isTimeFormatOn = std::stoi(line); } // TimeFormat
			else if (i == 3) { SetIp(line); } // Domain Ip
			else if (i == 4) { SetPort(line); } // Domain Port
			
 
			i++;
		}
		initFile.close();

		if(isInitialised)
		{

			if (!LanguageFileInitialiser::initialisedLanguages[currentLanguage])
			{
				LanguageFileInitialiser::GenerateLanguageFile(currentLanguage);
			}

			if (!LanguageFileInitialiser::PopulateAllTextsInApplication())
			{
				isSuccessful = false;
			}
		}
		else
		{
			Initialise(initialisedTxt, isSuccessful);
		}

		
	}

	return isSuccessful;
	



}

// Domain Config
std::string config::SDomainIp = "127.0.0.1";
std::wstring config::WDomainIp = std::wstring(config::SDomainIp.begin(), SDomainIp.end());
std::string config::SDomainPort = "4096";
int config::IDomainPort = 4096;

std::string config::currentDirUnNormalised = std::filesystem::current_path().string();
std::string config::currentDirNormalised = NormaliseDir(currentDirUnNormalised);

bool config::isInitialised = false;

int config::currentLanguage = 0;

bool config::isTimeFormatOn = false;



