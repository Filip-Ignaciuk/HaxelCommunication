#pragma once
#include "Chatroom.hpp"
#include "string"

inline static Chatroom emptyChatroom;

class NetworkCalls
{


public:
	virtual void CreateSocket();
	virtual void CloseSocket();
	virtual void Connect(const std::string& _ip, const int _port, std::string& _password);
	virtual void Disconnect();

	virtual void SendText(std::string _message);
	virtual void UpdateUser();
	virtual void Receive();

	virtual bool GetReceivingStatus();
	virtual bool GetChatroomStatus();
	virtual bool GetConnectedStatus();
	virtual Chatroom& GetChatroom();

	virtual ~NetworkCalls();
};