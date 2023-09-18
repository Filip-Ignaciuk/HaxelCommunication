
#pragma once
#include "config.hpp"

class LanguageFileInitialiser
{
private:
	// Consts
	static const unsigned char numberOfSentences = 30;
	static unsigned char numberOfLanguages = 2;
	static const std::string languages[numberOfLanguages] = { "/en-gb.txt", "/pl.txt" };
	inline static const std::string languagePath = config::currentDirNormalised + "/Languages";

	// Generate specific language
	static bool GenerateEnGb();
	static bool GeneratePl();

public:
	// Currently generated languages
	static bool initialisedLanguages[numberOfLanguages];

	static char* charAllTextsInApplication[numberOfLanguages];
	static std::string allTextsInApplication[numberOfSentences];

	static bool GenerateLanguageFile(const int _language);

	static bool CheckInstalledLanguages();

	static bool PopulateAllTextsInApplication();

};