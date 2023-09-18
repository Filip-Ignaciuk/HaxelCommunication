#include <fstream>
#include <filesystem>

#include "Languages.hpp"
#include "config.hpp"


bool LanguageFileInitialiser::GenerateLanguageFile(const int _language)
{
	CheckInstalledLanguages();

	bool isSuccessful = true;

	if (!_language) { isSuccessful = GenerateEnGb(); }
	else if (_language == 1) { isSuccessful = GeneratePl(); }
	else { isSuccessful = false; }

	return isSuccessful;
}

bool LanguageFileInitialiser::GenerateEnGb()
{
	bool isSuccessful = true;
	std::ofstream fileengb(languagePath + languages[0])
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

}

bool LanguageFileInitialiser::CheckInstalledLanguages()
{
	for (const auto& entry : std::filesystem::directory_iterator(languagePath))
	{
		std::string entryReversed = entry.path().string();
		std::reverse(entryReversed.begin(), entryReversed.end());
		std::string fileName;
		int i = 0;
		while(fileName[i] != '/')
		{
			fileName.push_back(fileName[i]);
			i++;
		}
		fileName.push_back(fileName[i]);

		for (i = 0; i < numberOfLanguages; i++)
		{
			if(fileName == languages[i])
			{
				initialisedLanguages[i] = true;
			}
		}
	}
}

bool LanguageFileInitialiser::PopulateAllTextsInApplication()
{
	if(initialisedLanguages[config::currentLanguage])
	{
		const std::string path = languagePath + languages[config::currentLanguage];
		std::ifstream languageFile(path);
		if(languageFile.is_open())
		{
			std::string line;
			int i = 0;
			while(std::getline(languageFile, line))
			{
				allTextsInApplication[i] = line;
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
