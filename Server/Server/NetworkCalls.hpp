#pragma once
#include <map>
#include <WinSock2.h>

#include "Chatroom.hpp"
#include "string"

inline static Chatroom emptyChatroom;


class NetworkCalls
{
public:
	// Fundamental Procedures
	virtual ~NetworkCalls();
	virtual void CreateSocket();
	virtual void CloseSocket();
	virtual void Bind(char* _ip, int _port);
	virtual void Listen();
	virtual void Receive();
	virtual Chatroom* GetChatroom();
	virtual void OpenChatroom(char* _chatroomName, char* _chatroomPassword);
	virtual void CloseChatroom();
	virtual void Disconnect();

	virtual bool GetListeningStatus();
	virtual bool GetBindStatus();
	virtual bool GetChatroomStatus();
	virtual const char* GetCurrentIp();
	virtual int GetCurrentPort();
};