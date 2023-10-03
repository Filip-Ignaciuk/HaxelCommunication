
#pragma once
#include "config.hpp"

class LanguageFileInitialiser
{
private:
	// Consts
	static const unsigned int numberOfSentences = 31;
	inline static const unsigned int numberOfLanguages = 2;
	inline static const std::string languagesExtention[numberOfLanguages] = { "/en-gb.txt", "/pl.txt" };
	inline static const std::string reversedLanguagesExtention[numberOfLanguages] = { "txt.bg-ne/", "txt.lp/" };

	// Generate specific language
	static bool GenerateEnGb();
	static bool GeneratePl();

public:
	// Currently generated languages
	static bool initialisedLanguages[numberOfLanguages];

	static char* charAllTextsInApplication[numberOfSentences];
	static std::string allTextsInApplication[numberOfSentences];

	static bool GenerateLanguageFile(const int _language);
	static bool ChangeLanguage(int _language);

	static bool CheckInstalledLanguages();

	static bool PopulateAllTextsInApplication();

};