#pragma once
#include <string>

#include "Chatroom.hpp"


class config
{
private:

	static bool m_isInitialised;
	static int m_currentWindow;
	static int m_currentLanguage;

	static Chatroom m_chatroom;

	static void Initialise(const std::string& _txt);
	static void InitFolders();

public:
	static std::string NormaliseDir(std::string& _str);

	static void StartConfigs();

	static void UpdateLanguage(int _language);

	static int GetCurrentLanguage();

	static void SetIp(std::string& _ip);

	static void SetPort(std::string& _port);

	static void ChangeWindow(int _window);

	static bool GetIsInitialised();
	static int GetCurrentWindow();

	static void SetChatroom(Chatroom& _chatroom);
	static Chatroom& GetChatroom();

	
	

	// For requesting a chatroom with a specific domain. Not to be confused with the actual domain server ip/port
	static std::string SDomainIp;
	static std::wstring WDomainIp;
	static std::string SDomainPort;
	static int IDomainPort;

	static std::string currentDirNormalised;
	static std::string currentDirUnNormalised;

};

