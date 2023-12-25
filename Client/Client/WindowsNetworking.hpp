#pragma once
#include "stdafx.h"
#include <WinSock2.h>
#include <WS2tcpip.h>

// Application Headers
#include "NetworkCalls.hpp"

struct ConnectHolder
{
	const std::wstring ip;
	const int port;
	std::string password;
};

// Implementation of NetworkCalls in windows.
class WindowsNetworking : public NetworkCalls
{
private:
	static SOCKET clientSocket;

	static bool isReceiving;

	static bool isConnected;

	static bool inChatroom;

	static DWORD WINAPI ConnectThread(LPVOID param);
	static DWORD WINAPI DisconnectThread(LPVOID param);

	// Send
	static DWORD WINAPI SendTextThread(LPVOID param);
	static DWORD WINAPI UpdateUserThread(LPVOID param);

	// Receive
	static DWORD WINAPI ReceiveSendMessageThread(LPVOID param);
	static DWORD WINAPI ReceiveConnect(LPVOID param);
	static DWORD WINAPI ReceiveUserUpdateThread(LPVOID param);
	static DWORD WINAPI ReceiveTextThread(LPVOID param);

	static DWORD WINAPI ReceiveThread(LPVOID param);
	
	
public:
	WindowsNetworking();

	// Fundamental Procedures
	void CreateSocket() override;
	void CloseSocket() override;
	void Connect(const std::string& _ip, int _port, std::string& _password) override;
	void Disconnect() override;

	// Main Procedures
	void SendText(const std::string& _message) override;
	void UpdateUser() override;
	void Receive() override;

	// Class Based
	bool GetReceivingStatus() override;
	bool GetConnectionStatus() override;
	bool GetChatroomStatus() override;

};