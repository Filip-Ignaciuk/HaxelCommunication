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

	// Obtaining file path.
	currentDirNormalised = std::filesystem::current_path().string();
	currentDirNormalised = NormaliseDir(currentDirNormalised);

	
	const std::string initialisedTxt = currentDirNormalised + "/initialised.txt";
	std::ifstream initFile(initialisedTxt);
	if (!initFile.is_open())
	{
		initFile.close();
		std::ofstream file(initialisedTxt);
		file << "1" << std::endl; // Is Application initialised
		file << "0" << std::endl; // Language
		file << "1" << std::endl; // TimeFormat

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
			if (i == 1) { currentLanguage = std::stoi(line); }
			else if (i == 2) { currentLanguage = std::stoi(line); }
			else if (i == 3) { isTimeFormatOn = std::stoi(line); }
			

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





