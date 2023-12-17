#pragma once
#include "stdafx.h"
#include <WinSock2.h>
#include <WS2tcpip.h>

// Application Headers
#include "Chatroom.hpp"
#include "NetworkCalls.hpp"

struct ConnectHolder
{
	std::wstring* ip;
	int* port;
};

// Implementation of NetworkCalls in windows.
class WindowsNetworking : public NetworkCalls
{
private:
	static SOCKET serverSocket;

	static bool isReceiving;

	static bool isBinded;

	static bool inChatroom;

	static Chatroom chatroom;

	static std::wstring currentWideIp;
	static std::string currentIp;
	static int currentPort;

	static DWORD WINAPI ListenThread(LPVOID param);
	static DWORD WINAPI BindThread(LPVOID param);
	static DWORD WINAPI DisconnectThread(LPVOID param);

	static DWORD WINAPI SendTextThread(LPVOID param);
	static DWORD WINAPI UpdateUserThread(LPVOID param);

	static DWORD WINAPI ReceiveSendMessageThread(LPVOID param);
	static DWORD WINAPI ReceiveUserUpdateThread(LPVOID param);
	static DWORD WINAPI ReceiveTextThread(LPVOID param);

	static DWORD WINAPI ReceiveThread(LPVOID param);
	
	
public:
	WindowsNetworking();

	// Fundamental Procedures
	void CreateSocket() override;
	void CloseSocket() override;
	void Bind(const std::string& _ip, int _port) override;
	Chatroom* GetChatroom() override;
	void OpenChatroom() override;
	void CloseChatroom() override;
	void Disconnect() override;

	// Main Procedures
	void SendText(const std::string& _message) override;
	void UpdateUser() override;
	void Receive() override;

	// Class Based
	bool GetReceivingStatus() override;
	bool GetBindStatus() override;
	bool GetChatroomStatus() override;
    const char* GetCurrentIp() override;
	int GetCurrentPort() override;
};