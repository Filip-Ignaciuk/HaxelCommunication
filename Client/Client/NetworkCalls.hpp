#pragma once
#include "string"

class NetworkCalls
{
public:
	virtual void CreateSocket();
	virtual void CloseSocket();
	virtual void Connect(const std::string& _ip, const int _port);
	virtual void Disconnect();

	virtual void SendText(const std::string& _message);
	virtual void UpdateUser();
	virtual void Receive();

	virtual bool GetReceivingStatus();
	virtual bool GetConnectionStatus();
	virtual bool GetChatroomStatus();

	virtual ~NetworkCalls();
};