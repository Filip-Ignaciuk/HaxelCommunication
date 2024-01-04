#pragma once
#include "NetworkCalls.hpp"

class NetworkCallsCreator
{
public:
	// Fundamental Procedures
	virtual NetworkCalls* CreateNetworkCalls() const = 0;
	virtual ~NetworkCallsCreator();
	void CreateSocket() const;
	void CloseSocket() const;
	void Bind(const std::string& _ip, int _port) const;
	void Listen();
	void Receive();
	Chatroom* GetChatroom();
	void OpenChatroom(std::string& _chatroomName, std::string& _chatroomPassword);
	void CloseChatroom();
	void Disconnect() const;

	// Main Procedures
	void UpdateTexts();
	void UpdateUsers();
	// Class Based
	bool GetListeningStatus() const;
	bool GetBindStatus() const;
	bool GetChatroomStatus() const;
	const char* GetCurrentIp() const;
	int GetCurrentPort() const;
};