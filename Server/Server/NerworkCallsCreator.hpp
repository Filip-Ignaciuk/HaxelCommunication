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
	void Bind(char* _ip, int _port);
	void Listen();
	void Receive();
	Chatroom* GetChatroom();
	void OpenChatroom(char* _chatroomName, char* _chatroomPassword);
	void CloseChatroom();
	void Disconnect() const;

	// Class Based
	bool GetListeningStatus() const;
	bool GetBindStatus() const;
	bool GetChatroomStatus() const;
	const char* GetCurrentIp() const;
	int GetCurrentPort() const;
};