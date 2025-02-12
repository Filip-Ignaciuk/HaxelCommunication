﻿#include <fstream>
#include <filesystem>

#include "Languages.hpp"

#include "Config.hpp"
#include "Error.hpp"
#include "ErrorHandler.hpp"


void LanguageFileInitialiser::GenerateLanguageFile(const int _language)
{
	
	// No need for else as we already checked for maximums and minimums.
	// Returns the result if it has successfully initialised.
	if (!_language && !initialisedLanguages[_language]) { initialisedLanguages[_language] = GenerateEnGb(); }
	else if (_language == 1 && !initialisedLanguages[_language]) { initialisedLanguages[_language] = GeneratePl(); }
}

void LanguageFileInitialiser::ChangeLanguage(const int _language)
{
	if(_language < 0 || numberOfLanguages < _language  )
	{
		// number is outside range
		Error invalidLanguage(charAllTextsInApplication[30], 1);
		ErrorHandler::AddError(invalidLanguage);
	}

	if(initialisedLanguages[_language] == false)
	{
		GenerateLanguageFile(_language);
		PopulateAllTextsInApplication();
	}
}

bool LanguageFileInitialiser::GenerateEnGb()
{
	const std::string languagePath = config::currentDirNormalised + "/Languages";
	std::ofstream fileengb(languagePath + languagesExtention[0]);
	if(fileengb.is_open())
	{
		fileengb << "Haxel Communication" << std::endl;
		fileengb << "Chatroom" << std::endl;
		fileengb << "Users" << std::endl;
		fileengb << "Settings" << std::endl;
		fileengb << "Language" << std::endl;
		fileengb << "Apperance " << std::endl;
		fileengb << "Credit" << std::endl;
		fileengb << "Chatroom network details:" << std::endl;
		fileengb << "IP" << std::endl;
		fileengb << "Port" << std::endl;
		fileengb << "Create Chatroom" << std::endl;
		fileengb << "Critical" << std::endl;
		fileengb << "Close Program" << std::endl;
		fileengb << "Warning" << std::endl;
		fileengb << "Cancel" << std::endl;
		fileengb << "Information" << std::endl;
		fileengb << "Invalid IP" << std::endl;
		fileengb << "Invalid Port" << std::endl;
		fileengb << "Close Chatroom" << std::endl;
		fileengb << "Close Socket" << std::endl;
		fileengb << "Are you sure you would like to close the chatroom?" << std::endl;
		fileengb << "Are you sure you would like to unbind from your chosen IP and Port?" << std::endl;
		fileengb << "Yes" << std::endl;
		fileengb << "No" << std::endl;
		fileengb << "Could not bind to specified Ip/Port." << std::endl;
		fileengb << "Couldn't create socket. Wsa startup failed." << std::endl;
		fileengb << "Couldn't create socket." << std::endl;
		fileengb << "Couldn't close socket." << std::endl;
		fileengb << "Failed to create folders for language files." << std::endl;
		fileengb << "Failed to create folders for user files." << std::endl;
		fileengb << "Tried to change to a language that isn't supported." << std::endl;
		fileengb << "Failed to create EnGB text file." << std::endl;
		fileengb << "Tried to read a language File that doesn't exist, regenerating file." << std::endl;
		fileengb << "Tried to update non-existent User." << std::endl;
		fileengb << "Binded" << std::endl;
		fileengb << "Not Binded" << std::endl;
		fileengb << "Chatroom Name" << std::endl;
		fileengb << "Change Chatroom" << std::endl;
		fileengb << "Listening..." << std::endl;
		fileengb << "Accepted Socket" << std::endl;
		fileengb << "Declined Socket, maximum numbers of users reached." << std::endl;
		fileengb << "Enter Command" << std::endl;
		fileengb << "Online" << std::endl;
		fileengb.close();
	}
	else
	{
		Error failedToCreateLanguageFile(LanguageFileInitialiser::charAllTextsInApplication[31], 0);
		ErrorHandler::AddError(failedToCreateLanguageFile);
		return false;
	}

	return true;
	
}

bool LanguageFileInitialiser::GeneratePl()
{


	return true;
}

void LanguageFileInitialiser::UpdateInstalledLanguages()
{
	const std::string languagePath = config::currentDirNormalised + "/Languages";
	for (const auto& entry : std::filesystem::directory_iterator(languagePath))
	{
		// Reversing the directory so that we can get the name of the document.
		// Bad code need to redo.
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

}

void LanguageFileInitialiser::PopulateAllTextsInApplication()
{
	const std::string languagePath = config::currentDirNormalised + "/Languages";
	
	if(!initialisedLanguages[config::GetCurrentLanguage()])
	{
		Error NoLanguageFile(charAllTextsInApplication[32], 1);
		ErrorHandler::AddError(NoLanguageFile);
		GenerateLanguageFile(config::GetCurrentLanguage());
	}

	const std::string path = languagePath + languagesExtention[config::GetCurrentLanguage()];
	std::ifstream languageFile(path);
	if (!languageFile.is_open())
	{
		// System thinks there is a language file and that it is initialised.
		Error NoLanguageFile(charAllTextsInApplication[32], 1);
		ErrorHandler::AddError(NoLanguageFile);
		GenerateLanguageFile(config::GetCurrentLanguage());

	}

	std::string line;
	int i = 0;
	while (std::getline(languageFile, line))
	{
		allTextsInApplication[i] = line;
		charAllTextsInApplication[i] = _strdup(line.c_str());
		i++;
	}
}


bool LanguageFileInitialiser::initialisedLanguages[numberOfLanguages];
char* LanguageFileInitialiser::charAllTextsInApplication[numberOfSentences];
std::string LanguageFileInitialiser::allTextsInApplication[numberOfSentences];