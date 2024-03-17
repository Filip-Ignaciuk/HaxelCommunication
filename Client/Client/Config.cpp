#include <string>
#include <filesystem>
#include <fstream>

#include "config.hpp"
#include "ErrorHandler.hpp"
#include "Languages.hpp"

bool config::m_isInitialised = false;
int config::m_currentLanguage = 0;

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
	if (!std::filesystem::exists(currentDirNormalised + "/Languages"))
	{
		if (!std::filesystem::create_directory(currentDirNormalised + "/Languages"))
		{
			Error FileSystemError(LanguageFileInitialiser::charAllTextsInApplication[28], 0);
			ErrorHandler::AddError(FileSystemError);
		}
	}

	if (!std::filesystem::exists(currentDirNormalised + "/Users"))
	{
		if (!std::filesystem::create_directory(currentDirNormalised + "/Users"))
		{
			Error FileSystemError(LanguageFileInitialiser::charAllTextsInApplication[29], 0);
			ErrorHandler::AddError(FileSystemError);
		}
	}



}

void config::Initialise(const std::string& _txt)
{
	std::ofstream file(_txt);
	file << "1" << std::endl; // Is Application initialised
	file << "0" << std::endl; // Language, default is english
	file << "0" << std::endl;

	m_currentLanguage = 0;

	// Initialising folders.
	InitFolders();

	// Initialising Language File.
	file.close();
	LanguageFileInitialiser::ChangeLanguage(0);
	m_isInitialised = 1;
}

void config::UpdateLanguage(const int _language)
{
	LanguageFileInitialiser::ChangeLanguage(_language);
}

int config::GetCurrentLanguage()
{
	return m_currentLanguage;
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
		std::string line;
		int i = 0;
		while (std::getline(initFile, line))
		{
			if (i == 0) { m_isInitialised = std::stoi(line); } // Is Application initialised
			else if (i == 1) { m_currentLanguage = std::stoi(line); } // Language


			i++;
		}
		initFile.close();


		LanguageFileInitialiser::ChangeLanguage(m_currentLanguage);


	}


}


bool config::GetIsInitialised()
{
	return m_isInitialised;
}



std::string config::currentDirUnNormalised = std::filesystem::current_path().string();
std::string config::currentDirNormalised = NormaliseDir(currentDirUnNormalised);
