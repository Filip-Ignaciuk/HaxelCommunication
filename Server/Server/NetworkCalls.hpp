#pragma once
#include "Chatroom.hpp"
#include "string"

class NetworkCalls
{
public:
	// Fundamental Procedures
	virtual ~NetworkCalls();
	virtual void CreateSocket();
	virtual void CloseSocket();
	virtual void Bind(const std::string& _ip, int _port);
	virtual void Listen();
	virtual Chatroom* GetChatroom();
	virtual void OpenChatroom();
	virtual void CloseChatroom();
	virtual void Disconnect();

	virtual void SendText(const std::string& _message);
	virtual void UpdateUser();

	virtual bool GetListeningStatus();
	virtual bool GetBindStatus();
	virtual bool GetChatroomStatus();
	virtual const char* GetCurrentIp();
	virtual int GetCurrentPort();
};