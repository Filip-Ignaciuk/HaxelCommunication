#pragma once
#include "string"

class NetworkCalls
{
public:
	virtual bool CreateSocket();
	virtual bool CloseSocket();

	virtual void Connect(const std::string& _ip, const int _port);
	virtual void SendText(const std::string& _message);
	virtual void UpdateUser();
	virtual void Receive();

	virtual bool GetReceiving();

	virtual ~NetworkCalls();
};