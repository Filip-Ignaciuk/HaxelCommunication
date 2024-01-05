#pragma once
#include "Chatroom.hpp"
#include "string"

class NetworkCalls
{


public:
	virtual void CreateSocket();
	virtual void CloseSocket();
	virtual void Connect(const std::string& _ip, const int _port, std::string& _password);
	virtual void Disconnect();

	virtual void SendText(std::string _message);
	virtual void UpdateUser(User& _user);
	virtual void Receive();

	virtual bool GetReceivingStatus();
	virtual bool GetChatroomStatus();
	virtual bool GetConnectedStatus();
	virtual bool GetUpdatedUserStatus();

	virtual Chatroom& GetChatroom();


	virtual ~NetworkCalls();
};