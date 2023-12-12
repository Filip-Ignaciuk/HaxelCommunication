#pragma once
#include "string"

class NetworkCalls
{
public:
	virtual void CreateSocket();
	virtual void CloseSocket();
	virtual void Bind(const std::string& _ip, int _port);
	virtual void Disconnect();

	virtual void SendText(const std::string& _message);
	virtual void UpdateUser();
	virtual void Receive();

	virtual bool GetReceivingStatus();
	virtual bool GetBindStatus();
	virtual bool GetChatroomStatus();

	virtual ~NetworkCalls();
};