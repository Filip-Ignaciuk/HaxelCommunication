#pragma once
#include "stdafx.h"
#include <WinSock2.h>
#include <WS2tcpip.h>

#include "NetworkCalls.hpp"

struct ConnectHolder
{
	const std::wstring ip;
	const int port;
};

// Implementation of NetworkCalls in windows.
class WindowsNetworking : public NetworkCalls
{
private:
	static SOCKET clientSocket;

	static bool isReceiving;

	static DWORD WINAPI ConnectThread(LPVOID param);
	static DWORD WINAPI TryToConnectThread(LPVOID param);
	static DWORD WINAPI SendTextThread(LPVOID param);
	static DWORD WINAPI UpdateUserThread(LPVOID param);
	static DWORD WINAPI ReceiveThread(LPVOID param);
	
	
public:
	WindowsNetworking();

	// Fundamental Functions
	bool CreateSocket() override;
	bool CloseSocket() override;

	// Common Procedures
	void Connect(const std::string& _ip, int _port) override;
	void SendText(const std::string& _message) override;
	void UpdateUser() override;
	void Receive() override;

	// Class Based
	bool GetReceiving() override;

};