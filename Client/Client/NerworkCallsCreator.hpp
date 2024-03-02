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
	void Connect(char* _ip, const int _port, char* _password);
	void Disconnect() const;

	// Main Procedures
	void SendText(char* _message) const;
	void UpdateUser(User* _user) const;
	void Receive();

	bool GetReceivingStatus();
	bool GetChatroomStatus();
	bool GetConnectedStatus();
	bool GetUpdatedUserStatus();
	Chatroom& GetChatroom();
};