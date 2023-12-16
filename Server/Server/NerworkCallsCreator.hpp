#pragma once
#include "NetworkCalls.hpp"

class NetworkCallsCreator
{
public:
	virtual NetworkCalls* CreateNetworkCalls() const = 0;
	virtual ~NetworkCallsCreator();
	void CreateSocket() const;
	void CloseSocket() const;
	void Bind(const std::string& _ip, int _port) const;
	void Disconnect() const;

	// Main Procedures
	void SendText(const std::string& _message) const;
	void UpdateUser() const;
	void Receive() const;

	// Class Based
	bool GetReceivingStatus() const;
	bool GetBindStatus() const;
	bool GetChatroomStatus() const;
	const char* GetCurrentIp() const;
	int GetCurrentPort() const;
};