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
	void Connect(const std::string& _ip, const int _port, std::string& _password);
	void Disconnect() const;

	// Main Procedures
	void SendText(std::string _message) const;
	void UpdateUser() const;
	void Receive() const;

	bool GetReceivingStatus();
	bool GetChatroomStatus();
	bool GetConnectedStatus();
	Chatroom& GetChatroom();
};