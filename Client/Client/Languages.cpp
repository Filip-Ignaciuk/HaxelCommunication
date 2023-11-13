#include <fstream>
#include <filesystem>

#include "Languages.hpp"
#include "config.hpp"


bool LanguageFileInitialiser::GenerateLanguageFile(const int _language)
{
	CheckInstalledLanguages();

	bool isSuccessful = true;

	if (!_language && !initialisedLanguages[_language]) { initialisedLanguages[_language] = GenerateEnGb(); }
	else if (_language == 1 && !initialisedLanguages[_language]) { initialisedLanguages[_language] = GeneratePl(); }
	else { isSuccessful = false; }

	return isSuccessful;
}

bool LanguageFileInitialiser::ChangeLanguage(const int _language)
{
	bool isSuccessful = true;
	if(_language < 0 || _language > numberOfLanguages  )
	{
		return false;
	}
	config::currentLanguage = _language;
	if(initialisedLanguages[_language] == false)
	{
		isSuccessful = GenerateLanguageFile(_language);
		isSuccessful = PopulateAllTextsInApplication();
	}
	return isSuccessful;
}

bool LanguageFileInitialiser::GenerateEnGb()
{
	const std::string languagePath = config::currentDirNormalised + "/Languages";
	bool isSuccessful = true;
	std::ofstream fileengb(languagePath + languagesExtention[0]);
	if(fileengb.is_open())
	{
		fileengb << "IP" << std::endl;
		fileengb << "Port" << std::endl;
		fileengb << "Connection" << std::endl;
		fileengb << "Not connected" << std::endl;
		fileengb << "Connected" << std::endl;
		fileengb << "Connect " << std::endl;
		fileengb << "Failed to connect" << std::endl;
		fileengb << "Connecting." << std::endl;
		fileengb << "Connecting.." << std::endl;
		fileengb << "Connecting..." << std::endl;
		fileengb << "User" << std::endl;
		fileengb << "Display Name" << std::endl;
		fileengb << "Changing." << std::endl;
		fileengb << "Changing.." << std::endl;
		fileengb << "Changing..." << std::endl;
		fileengb << "Successfully initialised user" << std::endl;
		fileengb << "Update user" << std::endl;
		fileengb << "Chat room" << std::endl;
		fileengb << "In chat room" << std::endl;
		fileengb << "Type /h for help" << std::endl;
		fileengb << "Message" << std::endl;
		fileengb << "Successfully changed user data" << std::endl;
		fileengb << "Languages" << std::endl;
		fileengb << "Display Name is too large" << std::endl;
		fileengb << "Invalid port or IP" << std::endl;
		fileengb << "Max clients within chatroom! Please wait for someone to leave or join another one." << std::endl;
		fileengb << "Request timed out. Please try to reconnect to the server." << std::endl;
		fileengb << "The server has been disconnected." << std::endl;
		fileengb << "Failed to connect to the domain server." << std::endl;
		fileengb << "Chatroom doesn't exist with chosen name." << std::endl;
		fileengb << "Password incorrect for the requested chatroom." << std::endl;
		fileengb << "User Information" << std::endl;
		fileengb << "User Customisation" << std::endl;
		fileengb.close();
	}
	else
	{
		isSuccessful = false;
	}

	return isSuccessful;
	
}

bool LanguageFileInitialiser::GeneratePl()
{


	return true;
}

bool LanguageFileInitialiser::CheckInstalledLanguages()
{
	const std::string languagePath = config::currentDirNormalised + "/Languages";
	for (const auto& entry : std::filesystem::directory_iterator(languagePath))
	{
		std::string entryReversedNormalised = entry.path().string();
		entryReversedNormalised = config::NormaliseDir(entryReversedNormalised);
		std::reverse(entryReversedNormalised.begin(), entryReversedNormalised.end());
		std::string fileName;
		int i = 0;
		while(entryReversedNormalised[i] != '/')
		{
			fileName.push_back(entryReversedNormalised[i]);
			i++;
		}
		fileName.push_back(entryReversedNormalised[i]);

		for (i = 0; i < numberOfLanguages; i++)
		{
			if(fileName == reversedLanguagesExtention[i])
			{
				initialisedLanguages[i] = true;
				break;
			}
		}
	}

	return true;
}

bool LanguageFileInitialiser::PopulateAllTextsInApplication()
{
	const std::string languagePath = config::currentDirNormalised + "/Languages";
	if(initialisedLanguages[config::currentLanguage])
	{
		const std::string path = languagePath + languagesExtention[config::currentLanguage];
		std::ifstream languageFile(path);
		if(languageFile.is_open())
		{
			std::string line;
			int i = 0;
			while(std::getline(languageFile, line))
			{
				allTextsInApplication[i] = line;
				charAllTextsInApplication[i] = _strdup(line.c_str());
				i++;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}


bool LanguageFileInitialiser::initialisedLanguages[numberOfLanguages];
char* LanguageFileInitialiser::charAllTextsInApplication[numberOfSentences];
std::string LanguageFileInitialiser::allTextsInApplication[numberOfSentences];