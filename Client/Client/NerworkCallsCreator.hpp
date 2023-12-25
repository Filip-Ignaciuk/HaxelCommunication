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
	void Disconnect() const;

	// Main Procedures
	void SendText(const std::string& _message) const;
	void UpdateUser() const;
	void Receive() const;

	bool GetReceivingStatus();
	bool GetConnectionStatus();
	bool GetChatroomStatus();
};