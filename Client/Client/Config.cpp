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



bool config::StartConfigs()
{
	bool isSuccessful = true;
	
	const std::string initialisedTxt = currentDirNormalised + "/initialised.txt";
	std::ifstream initFile(initialisedTxt);
	if (!initFile.is_open())
	{
		initFile.close();
		std::ofstream file(initialisedTxt);
		file << "1" << std::endl; // Is Application initialised
		file << "0" << std::endl; // Language
		file << "1" << std::endl; // TimeFormat
		file << "127.0.0.1" << std::endl; // Domain Ip
		file << "4096" << std::endl; // Domain Port

		currentLanguage = 0;
		isTimeFormatOn = true;

		// Initialising folders.
		isSuccessful = InitFolders();

		// Initialising Language File.
		file.close();
		LanguageFileInitialiser::GenerateLanguageFile(0);
		LanguageFileInitialiser::PopulateAllTextsInApplication();


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
			else if (i == 3) { DSC.SetIp(line); } // Domain Ip
			else if (i == 4) { DSC.SetPort(line); } // Domain Port
			

			i++;
		}

		if(!LanguageFileInitialiser::initialisedLanguages[currentLanguage])
		{
			LanguageFileInitialiser::GenerateLanguageFile(currentLanguage);
		}

		if(!LanguageFileInitialiser::PopulateAllTextsInApplication())
		{
			return false;
		}

		initFile.close();
	}

	return true;
	



}

std::string config::currentDirUnNormalised = std::filesystem::current_path().string();
std::string config::currentDirNormalised = NormaliseDir(currentDirUnNormalised);

bool config::isInitialised = false;

int config::currentLanguage = 0;

bool config::isTimeFormatOn = false;



