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
	SOCKET clientSocket;

	DWORD WINAPI ConnectThread(LPVOID param);
	DWORD WINAPI TryToConnectThread(LPVOID param);
	DWORD WINAPI SendTextThread(LPVOID param);
	DWORD WINAPI UpdateUserThread(LPVOID param);
	DWORD WINAPI ReceiveThread(LPVOID param);
	
	
public:
	WindowsNetworking();


	bool CreateSocket() override;
	bool CloseSocket() override;

	void Connect(const std::string& _ip, const int _port) override;
	void SendText(const std::string& _message) override;
	void UpdateUser() override;
	void Receive() override;
};